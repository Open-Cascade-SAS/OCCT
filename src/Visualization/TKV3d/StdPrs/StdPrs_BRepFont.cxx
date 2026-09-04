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

#include <BRepFont_Builder.hxx>
#include <BRepFont_Regularizer.hxx>
#include <Font_FTFont.hxx>
#include <Font_TextFormatter.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <gp_XY.hxx>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>
#include <variant>

IMPLEMENT_STANDARD_RTTIEXT(StdPrs_BRepFont, Standard_Transient)

namespace
{
// A 72-point font is one inch high. The high resolution reduces rounding in FreeType text metrics.
constexpr uint32_t THE_POINTS_PER_INCH = 72;
constexpr uint32_t THE_FONT_POINT_SIZE = 72;
constexpr uint32_t THE_FONT_RESOLUTION = 4800;
constexpr double   THE_FONT_METRIC_SIZE =
  static_cast<double>(THE_FONT_POINT_SIZE) * THE_FONT_RESOLUTION / THE_POINTS_PER_INCH;
constexpr Font_FTFont::Params THE_FONT_PARAMETERS{THE_FONT_POINT_SIZE, THE_FONT_RESOLUTION};

bool isValidReal(const double theValue) noexcept
{
  return !std::isnan(theValue) && !Precision::IsInfinite(theValue);
}

bool isValidPosition(const gp_Ax3& thePosition) noexcept
{
  const gp_Pnt& aLocation    = thePosition.Location();
  const gp_Dir& aDirection   = thePosition.Direction();
  const gp_Dir& anXDirection = thePosition.XDirection();
  const gp_Dir& anYDirection = thePosition.YDirection();
  return isValidReal(aLocation.X()) && isValidReal(aLocation.Y()) && isValidReal(aLocation.Z())
         && isValidReal(aDirection.X()) && isValidReal(aDirection.Y())
         && isValidReal(aDirection.Z()) && isValidReal(anXDirection.X())
         && isValidReal(anXDirection.Y()) && isValidReal(anXDirection.Z())
         && isValidReal(anYDirection.X()) && isValidReal(anYDirection.Y())
         && isValidReal(anYDirection.Z());
}
} // namespace

class StdPrs_BRepFont::Impl : public Standard_Transient
{
public:
  struct FileSource
  {
    TCollection_AsciiString Path;
    int                     FaceId = 0;
  };

  struct NamedSource
  {
    TCollection_AsciiString Name;
    Font_FontAspect         Aspect      = Font_FontAspect_Regular;
    Font_StrictLevel        StrictLevel = Font_StrictLevel_Any;
  };

  struct Metrics
  {
    double Size        = 0.0;
    double Scale       = 1.0;
    double Ascender    = 0.0;
    double Descender   = 0.0;
    double LineSpacing = 0.0;
    double PointSize   = 0.0;
  };

  using Source = std::variant<std::monostate, FileSource, NamedSource>;

  Impl()
      : Font(new Font_FTFont()),
        Tolerance(Precision::Confusion())
  {
  }

  struct Request
  {
    Font_GlyphOutline::Glyph Glyph;
    double                   Size;
  };

  struct PendingItem
  {
    Font_GlyphOutline::Glyph Glyph;
    gp_XY                    Position;
  };

  class GlyphCache
  {
  public:
    struct Entry
    {
      std::shared_ptr<const BRepFont_PlanarRegion> Region;
    };

    GlyphCache() { myEntries.ReSize(THE_CAPACITY); }

    [[nodiscard]] std::shared_ptr<const BRepFont_PlanarRegion> FindRegion(
      const Font_GlyphOutline::Glyph& theGlyph) const
    {
      const Entry* anEntry = myEntries.Seek(theGlyph);
      return anEntry != nullptr ? anEntry->Region : std::shared_ptr<const BRepFont_PlanarRegion>();
    }

    [[nodiscard]] std::optional<Font_GlyphOutline::Glyph> FindGlyph(
      const std::shared_ptr<const BRepFont_PlanarRegion>& theRegion) const
    {
      const Font_GlyphOutline::Glyph* aGlyph = myGlyphsByRegion.Seek(theRegion.get());
      return aGlyph != nullptr ? std::optional<Font_GlyphOutline::Glyph>(*aGlyph) : std::nullopt;
    }

    void BindRegion(const Font_GlyphOutline::Glyph&                     theGlyph,
                    const std::shared_ptr<const BRepFont_PlanarRegion>& theRegion)
    {
      if (myEntries.Contains(theGlyph) || !theRegion || !isCacheable(*theRegion))
      {
        return;
      }
      const Entry anEntry{theRegion};
      if (myEntries.Size() < THE_CAPACITY)
      {
        myEntries.Add(theGlyph, anEntry);
      }
      else
      {
        myGlyphsByRegion.UnBind(myEntries.FindFromIndex(myNextIndex).Region.get());
        myEntries.Substitute(myNextIndex, theGlyph, anEntry);
        myNextIndex = myNextIndex < THE_CAPACITY ? myNextIndex + 1 : 1;
      }
      myGlyphsByRegion.Bind(theRegion.get(), theGlyph);
    }

    void Clear()
    {
      myEntries.Clear();
      myGlyphsByRegion.Clear();
      myNextIndex = 1;
    }

  private:
    static bool isCacheable(const BRepFont_PlanarRegion& theRegion)
    {
      if (theRegion.Vertices().Size() > THE_MAX_REGION_COMPLEXITY
          || theRegion.Curves().Size() > THE_MAX_REGION_COMPLEXITY
          || theRegion.Uses().Size() > THE_MAX_REGION_COMPLEXITY
          || theRegion.Loops().Size() > THE_MAX_REGION_COMPLEXITY
          || theRegion.HoleLoops().Size() > THE_MAX_REGION_COMPLEXITY
          || theRegion.Regions().Size() > THE_MAX_REGION_COMPLEXITY)
      {
        return false;
      }
      const size_t aComplexity = theRegion.Vertices().Size() + theRegion.Curves().Size()
                                 + theRegion.Uses().Size() + theRegion.Loops().Size()
                                 + theRegion.HoleLoops().Size() + theRegion.Regions().Size();
      return aComplexity <= THE_MAX_REGION_COMPLEXITY;
    }

    static constexpr size_t THE_CAPACITY              = 256;
    static constexpr size_t THE_MAX_REGION_COMPLEXITY = 16384;
    NCollection_IndexedDataMap<Font_GlyphOutline::Glyph, Entry, Font_GlyphOutline::Glyph::Hasher>
                                                                                myEntries;
    NCollection_DataMap<const BRepFont_PlanarRegion*, Font_GlyphOutline::Glyph> myGlyphsByRegion;
    size_t                                                                      myNextIndex = 1;
  };

  void ClearCache()
  {
    std::unique_lock<std::shared_mutex> aLock(CacheMutex);
    Glyphs.Clear();
  }

  [[nodiscard]] occ::handle<Impl> Copy() const
  {
    std::lock_guard<std::mutex> aLock(FontMutex);
    occ::handle<Impl>           aCopy = new Impl();
    aCopy->Font->SetUseUnicodeSubsetFallback(Font->ToUseUnicodeSubsetFallback());
    bool isInitialized = false;
    if (const FileSource* aFileSource = std::get_if<FileSource>(&FontSource))
    {
      isInitialized =
        aCopy->Font->Init(aFileSource->Path.ToCString(), THE_FONT_PARAMETERS, aFileSource->FaceId);
    }
    else if (const NamedSource* aNamedSource = std::get_if<NamedSource>(&FontSource))
    {
      isInitialized = aCopy->Font->FindAndInit(aNamedSource->Name,
                                               aNamedSource->Aspect,
                                               THE_FONT_PARAMETERS,
                                               aNamedSource->StrictLevel);
    }
    else
    {
      isInitialized = !Font->IsValid();
    }
    if (!isInitialized)
    {
      return {};
    }
    aCopy->Font->SetSingleStrokeFont(Font->IsSingleStrokeFont());
    if (!aCopy->Font->SetWidthScaling(Font->WidthScaling()))
    {
      return {};
    }
    aCopy->SetSize(FontMetrics.Size);
    aCopy->FontSource = FontSource;
    return aCopy;
  }

  void SetSize(const double theSize)
  {
    const double aScale = theSize / THE_FONT_METRIC_SIZE;
    FontMetrics         = Metrics{theSize,
                          aScale,
                          aScale * static_cast<double>(Font->Ascender()),
                          aScale * static_cast<double>(Font->Descender()),
                          aScale * static_cast<double>(Font->LineSpacing()),
                          aScale * static_cast<double>(Font->PointSize())};
  }

  std::optional<Request> MakeRequest(const char32_t theChar)
  {
    std::lock_guard<std::mutex> aLock(FontMutex);
    if (!(FontMetrics.Size > 0.0) || !Font->IsValid())
    {
      return std::nullopt;
    }
    const std::optional<Font_GlyphOutline::Glyph> aGlyph = Font->ResolveGlyph(theChar);
    return aGlyph.has_value() ? std::optional<Request>(Request{*aGlyph, FontMetrics.Size})
                              : std::nullopt;
  }

  std::shared_ptr<const BRepFont_PlanarRegion> LoadRegion(const Request& theRequest)
  {
    std::shared_ptr<const BRepFont_PlanarRegion> aCachedRegion;
    {
      std::shared_lock<std::shared_mutex> aLock(CacheMutex);
      aCachedRegion = Glyphs.FindRegion(theRequest.Glyph);
    }
    if (aCachedRegion)
    {
      return aCachedRegion;
    }

    std::optional<Font_GlyphOutline> anOutline;
    {
      std::lock_guard<std::mutex> aLock(FontMutex);
      anOutline = Font->LoadGlyphOutline(theRequest.Glyph);
    }
    if (!anOutline.has_value())
    {
      return {};
    }

    BRepFont_Regularizer::Options anOptions;
    anOptions.Tolerance                  = Tolerance * anOutline->UnitsPerEm() / theRequest.Size;
    BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(*anOutline, anOptions);
    if (!aResult.IsDone() || !aResult.PlanarRegion().has_value())
    {
      return {};
    }
    std::shared_ptr<const BRepFont_PlanarRegion> aRegion =
      std::make_shared<BRepFont_PlanarRegion>(std::move(*aResult.ChangePlanarRegion()));

    std::lock_guard<std::mutex> aFontLock(FontMutex);
    if (!Font->IsGlyphValid(theRequest.Glyph))
    {
      return {};
    }
    std::unique_lock<std::shared_mutex> aCacheLock(CacheMutex);
    if (const std::shared_ptr<const BRepFont_PlanarRegion> anExisting =
          Glyphs.FindRegion(theRequest.Glyph))
    {
      return anExisting;
    }
    Glyphs.BindRegion(theRequest.Glyph, aRegion);
    return aRegion;
  }

  TopoDS_Shape RenderGlyph(const Request& theRequest, const bool theToConcatenateContours)
  {
    const std::shared_ptr<const BRepFont_PlanarRegion> aRegion = LoadRegion(theRequest);
    if (!aRegion)
    {
      return {};
    }

    BRepFont_Builder::GlyphOptions anOptions;
    anOptions.Size                  = theRequest.Size;
    anOptions.Tolerance             = Tolerance;
    anOptions.ToConcatenateContours = theToConcatenateContours;
    TopoDS_Shape aShape             = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);

    std::lock_guard<std::mutex> aFontLock(FontMutex);
    return Font->IsGlyphValid(theRequest.Glyph) ? aShape : TopoDS_Shape();
  }

  std::optional<TopoDS_Shape> RenderRegion(
    const std::shared_ptr<const BRepFont_PlanarRegion>& theRegion,
    const double                                        theSize,
    const bool                                          theToConcatenateContours)
  {
    std::optional<Font_GlyphOutline::Glyph> aGlyph;
    {
      std::shared_lock<std::shared_mutex> aLock(CacheMutex);
      aGlyph = Glyphs.FindGlyph(theRegion);
    }
    if (!aGlyph.has_value())
    {
      return std::nullopt;
    }
    return RenderGlyph(Request{*aGlyph, theSize}, theToConcatenateContours);
  }

  std::optional<BRepFont_Builder::TextPlan> PlanText(
    const NCollection_String&               theText,
    const Graphic3d_HorizontalTextAlignment theHorizontalAlignment,
    const Graphic3d_VerticalTextAlignment   theVerticalAlignment)
  {
    if (theText.IsEmpty())
    {
      return std::nullopt;
    }
    NCollection_LinearVector<PendingItem> aPendingItems;
    double                                aSize        = 0.0;
    double                                aMetricScale = 0.0;
    gp_XY                                 aLowerLeft;
    gp_XY                                 anUpperRight;
    {
      std::lock_guard<std::mutex> aLock(FontMutex);
      if (!(FontMetrics.Size > 0.0) || !Font->IsValid())
      {
        return std::nullopt;
      }
      aSize        = FontMetrics.Size;
      aMetricScale = FontMetrics.Scale;

      Font_TextFormatter aFormatter;
      aFormatter.SetupAlignment(theHorizontalAlignment, theVerticalAlignment);
      aFormatter.Append(theText, *Font);
      aFormatter.Format();
      Font_Rect aBounds;
      aFormatter.BndBox(aBounds);
      const double aLeft   = static_cast<double>(aBounds.Left) * aMetricScale;
      const double aRight  = static_cast<double>(aBounds.Right) * aMetricScale;
      const double aBottom = static_cast<double>(aBounds.Bottom) * aMetricScale;
      const double aTop    = static_cast<double>(aBounds.Top) * aMetricScale;
      aLowerLeft.SetCoord(std::min(aLeft, aRight), std::min(aBottom, aTop));
      anUpperRight.SetCoord(std::max(aLeft, aRight), std::max(aBottom, aTop));
      aPendingItems.Reserve(64);
      for (Font_TextFormatter::Iterator anIterator(
             aFormatter,
             Font_TextFormatter::IterationFilter_ExcludeInvisible);
           anIterator.More();
           anIterator.Next())
      {
        const std::optional<Font_GlyphOutline::Glyph> aGlyph =
          Font->ResolveGlyph(anIterator.Symbol());
        if (!aGlyph.has_value())
        {
          continue;
        }
        const NCollection_Vec2<float>& aCorner = aFormatter.BottomLeft(anIterator.SymbolPosition());
        aPendingItems.Append(PendingItem{*aGlyph,
                                         gp_XY(static_cast<double>(aCorner.x()) * aMetricScale,
                                               static_cast<double>(aCorner.y()) * aMetricScale)});
      }
    }
    NCollection_DataMap<Font_GlyphOutline::Glyph, size_t, Font_GlyphOutline::Glyph::Hasher>
                                                                           aRegionIndices;
    NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>> aRegions;
    NCollection_LinearVector<BRepFont_Builder::TextPlan::Item>             anItems;
    aRegions.Reserve(aPendingItems.Size());
    anItems.Reserve(aPendingItems.Size());
    for (const PendingItem& anItem : aPendingItems)
    {
      const size_t* aFoundIndex  = aRegionIndices.Seek(anItem.Glyph);
      size_t        aRegionIndex = 0;
      if (aFoundIndex == nullptr)
      {
        std::shared_ptr<const BRepFont_PlanarRegion> aRegion =
          LoadRegion(Request{anItem.Glyph, aSize});
        if (!aRegion)
        {
          continue;
        }
        aRegionIndex = aRegions.Size();
        aRegions.Append(std::move(aRegion));
        aRegionIndices.Bind(anItem.Glyph, aRegionIndex);
      }
      else
      {
        aRegionIndex = *aFoundIndex;
      }
      anItems.EmplaceAppend(aRegionIndex, anItem.Position);
    }
    return BRepFont_Builder::CreateTextPlan(aSize,
                                            Tolerance,
                                            aLowerLeft,
                                            anUpperRight,
                                            std::move(aRegions),
                                            std::move(anItems));
  }

public:
  occ::handle<Font_FTFont>  Font;
  GlyphCache                Glyphs;
  mutable std::mutex        FontMutex;
  mutable std::shared_mutex CacheMutex;
  const double              Tolerance;
  Metrics                   FontMetrics;
  Source                    FontSource;
};

//=================================================================================================

StdPrs_BRepFont::StdPrs_BRepFont()
    : myImpl(new Impl())
{
}

//=================================================================================================

StdPrs_BRepFont::StdPrs_BRepFont(const occ::handle<Impl>& theImpl)
    : myImpl(theImpl)
{
}

//=================================================================================================

StdPrs_BRepFont::StdPrs_BRepFont(const NCollection_String& theFontPath,
                                 const double              theSize,
                                 const int                 theFaceId)
    : StdPrs_BRepFont()
{
  Init(theFontPath, theSize, theFaceId);
}

//=================================================================================================

StdPrs_BRepFont::StdPrs_BRepFont(const NCollection_String& theFontName,
                                 const Font_FontAspect     theFontAspect,
                                 const double              theSize,
                                 const Font_StrictLevel    theStrictLevel)
    : StdPrs_BRepFont()
{
  FindAndInit(theFontName.ToCString(), theFontAspect, theSize, theStrictLevel);
}

//=================================================================================================

StdPrs_BRepFont::~StdPrs_BRepFont() = default;

//=================================================================================================

occ::handle<StdPrs_BRepFont> StdPrs_BRepFont::FindAndCreate(
  const TCollection_AsciiString& theFontName,
  const Font_FontAspect          theFontAspect,
  const double                   theSize,
  const Font_StrictLevel         theStrictLevel)
{
  occ::handle<StdPrs_BRepFont> aFont = new StdPrs_BRepFont();
  return aFont->FindAndInit(theFontName, theFontAspect, theSize, theStrictLevel)
           ? aFont
           : occ::handle<StdPrs_BRepFont>();
}

//=================================================================================================

void StdPrs_BRepFont::Release()
{
  float aWidthScaling = 1.0f;
  {
    std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
    aWidthScaling = myImpl->Font->WidthScaling();
  }
  occ::handle<Impl> aNewImpl = new Impl();
  aNewImpl->Font->SetWidthScaling(aWidthScaling);
  myImpl = aNewImpl;
}

//=================================================================================================

bool StdPrs_BRepFont::IsValid() const
{
  return myImpl->FontMetrics.Size > 0.0 && myImpl->Font->IsValid();
}

//=================================================================================================

bool StdPrs_BRepFont::Init(const NCollection_String& theFontPath,
                           const double              theSize,
                           const int                 theFaceId)
{
  float aWidthScaling = 1.0f;
  {
    std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
    aWidthScaling = myImpl->Font->WidthScaling();
  }
  occ::handle<Impl> aNewImpl = new Impl();
  if (!(theSize > aNewImpl->Tolerance * 100.0) || Precision::IsInfinite(theSize))
  {
    return false;
  }
  if (!aNewImpl->Font->Init(theFontPath.ToCString(), THE_FONT_PARAMETERS, theFaceId))
  {
    return false;
  }
  if (!aNewImpl->Font->SetWidthScaling(aWidthScaling))
  {
    return false;
  }
  aNewImpl->SetSize(theSize);
  aNewImpl->FontSource = Impl::FileSource{theFontPath.ToCString(), theFaceId};
  myImpl               = aNewImpl;
  return true;
}

//=================================================================================================

bool StdPrs_BRepFont::FindAndInit(const TCollection_AsciiString& theFontName,
                                  const Font_FontAspect          theFontAspect,
                                  const double                   theSize,
                                  const Font_StrictLevel         theStrictLevel)
{
  float aWidthScaling = 1.0f;
  {
    std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
    aWidthScaling = myImpl->Font->WidthScaling();
  }
  occ::handle<Impl> aNewImpl = new Impl();
  if (!(theSize > aNewImpl->Tolerance * 100.0) || Precision::IsInfinite(theSize))
  {
    return false;
  }
  if (!aNewImpl->Font->FindAndInit(theFontName, theFontAspect, THE_FONT_PARAMETERS, theStrictLevel))
  {
    return false;
  }
  if (!aNewImpl->Font->SetWidthScaling(aWidthScaling))
  {
    return false;
  }
  aNewImpl->SetSize(theSize);
  aNewImpl->FontSource = Impl::NamedSource{theFontName, theFontAspect, theStrictLevel};
  myImpl               = aNewImpl;
  return true;
}

//=================================================================================================

TopoDS_Shape StdPrs_BRepFont::RenderGlyph(const char32_t theChar)
{
  return RenderGlyph(theChar, GlyphOptions{});
}

//=================================================================================================

TopoDS_Shape StdPrs_BRepFont::RenderGlyph(const char32_t theChar, const GlyphOptions& theOptions)
{
  const std::optional<Impl::Request> aRequest = myImpl->MakeRequest(theChar);
  return aRequest.has_value() ? myImpl->RenderGlyph(*aRequest, theOptions.ToConcatenateContours)
                              : TopoDS_Shape();
}

//=================================================================================================

TopoDS_Shape StdPrs_BRepFont::RenderText(const NCollection_String& theText)
{
  return RenderText(theText, TextOptions{});
}

//=================================================================================================

TopoDS_Shape StdPrs_BRepFont::RenderText(const NCollection_String& theText,
                                         const TextOptions&        theOptions)
{
  if (!isValidPosition(theOptions.Pen))
  {
    return {};
  }
  std::optional<BRepFont_Builder::TextPlan> aPlan =
    PlanText(theText, theOptions.HorizontalAlignment, theOptions.VerticalAlignment);
  return aPlan.has_value() ? RenderText(*aPlan, theOptions) : TopoDS_Shape();
}

//=================================================================================================

std::optional<BRepFont_Builder::TextPlan> StdPrs_BRepFont::PlanText(
  const NCollection_String&               theText,
  const Graphic3d_HorizontalTextAlignment theHorizontalAlignment,
  const Graphic3d_VerticalTextAlignment   theVerticalAlignment)
{
  return myImpl->PlanText(theText, theHorizontalAlignment, theVerticalAlignment);
}

//=================================================================================================

TopoDS_Shape StdPrs_BRepFont::RenderText(const BRepFont_Builder::TextPlan& thePlan,
                                         const TextOptions&                theOptions)
{
  if (!isValidPosition(theOptions.Pen) || thePlan.NbRegions() == 0)
  {
    return {};
  }
  NCollection_LinearVector<TopoDS_Shape> aGlyphShapes(thePlan.NbRegions(), TopoDS_Shape{});
  for (size_t aRegionIndex = 0; aRegionIndex < thePlan.NbRegions(); ++aRegionIndex)
  {
    TopoDS_Shape&                     aGlyph = aGlyphShapes.ChangeValue(aRegionIndex);
    const std::optional<TopoDS_Shape> aCachedGlyph =
      myImpl->RenderRegion(thePlan.RegionHandle(aRegionIndex),
                           thePlan.Size(),
                           theOptions.ToConcatenateContours);
    if (aCachedGlyph.has_value())
    {
      aGlyph = *aCachedGlyph;
    }
    else
    {
      BRepFont_Builder::GlyphOptions aGlyphOptions;
      aGlyphOptions.Size                  = thePlan.Size();
      aGlyphOptions.Tolerance             = thePlan.Tolerance();
      aGlyphOptions.ToConcatenateContours = theOptions.ToConcatenateContours;
      aGlyph = BRepFont_Builder::BuildGlyph(thePlan.Region(aRegionIndex), aGlyphOptions);
    }
    if (aGlyph.IsNull() && !thePlan.Region(aRegionIndex).IsEmpty())
    {
      return {};
    }
  }
  BRepFont_Builder::TextOptions aBuilderOptions;
  aBuilderOptions.Pen                   = theOptions.Pen;
  aBuilderOptions.ToConcatenateContours = theOptions.ToConcatenateContours;
  return BRepFont_Builder::BuildText(thePlan, aGlyphShapes, aBuilderOptions);
}

//=================================================================================================

TopoDS_Shape StdPrs_BRepFont::RenderText(const BRepFont_Builder::TextPlan& thePlan)
{
  return RenderText(thePlan, TextOptions{});
}

//=================================================================================================

bool StdPrs_BRepFont::SetWidthScaling(const float theScaleFactor)
{
  if (!(theScaleFactor > 0.0f) || Precision::IsInfinite(static_cast<double>(theScaleFactor)))
  {
    return false;
  }
  {
    std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
    if (myImpl->Font->WidthScaling() == theScaleFactor)
    {
      return true;
    }
  }
  occ::handle<Impl> anImpl = myImpl->GetRefCount() == 1 ? myImpl : myImpl->Copy();
  if (anImpl.IsNull())
  {
    return false;
  }
  std::lock_guard<std::mutex> aLock(anImpl->FontMutex);
  if (!anImpl->Font->SetWidthScaling(theScaleFactor))
  {
    return false;
  }
  anImpl->ClearCache();
  myImpl = anImpl;
  return true;
}

//=================================================================================================

void StdPrs_BRepFont::SetSingleStrokeFont(const bool theIsSingleStroke)
{
  {
    std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
    if (myImpl->Font->IsSingleStrokeFont() == theIsSingleStroke)
    {
      return;
    }
  }
  occ::handle<Impl> anImpl = myImpl->GetRefCount() == 1 ? myImpl : myImpl->Copy();
  if (anImpl.IsNull())
  {
    return;
  }
  std::lock_guard<std::mutex> aLock(anImpl->FontMutex);
  anImpl->Font->SetSingleStrokeFont(theIsSingleStroke);
  anImpl->ClearCache();
  myImpl = anImpl;
}

//=================================================================================================

double StdPrs_BRepFont::Ascender() const
{
  return myImpl->FontMetrics.Ascender;
}

//=================================================================================================

double StdPrs_BRepFont::Descender() const
{
  return myImpl->FontMetrics.Descender;
}

//=================================================================================================

double StdPrs_BRepFont::LineSpacing() const
{
  return myImpl->FontMetrics.LineSpacing;
}

//=================================================================================================

double StdPrs_BRepFont::PointSize() const
{
  return myImpl->FontMetrics.PointSize;
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceX(const char32_t theUCharNext)
{
  std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
  return myImpl->FontMetrics.Scale * static_cast<double>(myImpl->Font->AdvanceX(theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceX(const char32_t theUChar, const char32_t theUCharNext)
{
  std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
  return myImpl->FontMetrics.Scale
         * static_cast<double>(myImpl->Font->AdvanceX(theUChar, theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceY(const char32_t theUCharNext)
{
  std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
  return myImpl->FontMetrics.Scale * static_cast<double>(myImpl->Font->AdvanceY(theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceY(const char32_t theUChar, const char32_t theUCharNext)
{
  std::lock_guard<std::mutex> aLock(myImpl->FontMutex);
  return myImpl->FontMetrics.Scale
         * static_cast<double>(myImpl->Font->AdvanceY(theUChar, theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::Scale() const
{
  return myImpl->FontMetrics.Scale;
}

//=================================================================================================

double StdPrs_BRepFont::Size() const
{
  return myImpl->FontMetrics.Size;
}
