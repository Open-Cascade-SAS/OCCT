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
#include <BRep_Builder.hxx>
#include <Font_FTFont.hxx>
#include <Font_FontMgr.hxx>
#include <Font_TextFormatter.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XY.hxx>

#include <cmath>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(StdPrs_BRepFont, Standard_Transient)

namespace
{
bool isFinite(const gp_Ax3& thePosition) noexcept
{
  const gp_Pnt& aLocation    = thePosition.Location();
  const gp_Dir& aDirection   = thePosition.Direction();
  const gp_Dir& anXDirection = thePosition.XDirection();
  const gp_Dir& anYDirection = thePosition.YDirection();
  return std::isfinite(aLocation.X()) && std::isfinite(aLocation.Y())
         && std::isfinite(aLocation.Z()) && std::isfinite(aDirection.X())
         && std::isfinite(aDirection.Y()) && std::isfinite(aDirection.Z())
         && std::isfinite(anXDirection.X()) && std::isfinite(anXDirection.Y())
         && std::isfinite(anXDirection.Z()) && std::isfinite(anYDirection.X())
         && std::isfinite(anYDirection.Y()) && std::isfinite(anYDirection.Z());
}
} // namespace

class StdPrs_BRepFont::Impl
{
public:
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
    Request GlyphRequest;
    gp_XY   Position;
  };

  class Cache
  {
  public:
    struct Entry
    {
      std::shared_ptr<const BRepFont_PlanarRegion> Region;
      TopoDS_Shape                                 DefaultShape;
      TopoDS_Shape                                 CompositeShape;
    };

    Cache()
    {
      myRegions.ReSize(THE_CAPACITY);
      myKeys.Reserve(THE_CAPACITY);
    }

    [[nodiscard]] std::shared_ptr<const BRepFont_PlanarRegion> Find(
      const Font_GlyphOutline::Glyph& theGlyph) const
    {
      const Entry* anEntry = myRegions.Seek(theGlyph);
      return anEntry != nullptr ? anEntry->Region : std::shared_ptr<const BRepFont_PlanarRegion>();
    }

    [[nodiscard]] TopoDS_Shape FindShape(const Font_GlyphOutline::Glyph& theGlyph,
                                         const bool theToConcatenateContours) const
    {
      const Entry* anEntry = myRegions.Seek(theGlyph);
      if (anEntry == nullptr)
      {
        return {};
      }
      const TopoDS_Shape& aShape =
        theToConcatenateContours ? anEntry->CompositeShape : anEntry->DefaultShape;
      return aShape;
    }

    void Bind(const Font_GlyphOutline::Glyph&                     theGlyph,
              const std::shared_ptr<const BRepFont_PlanarRegion>& theRegion)
    {
      if (myRegions.IsBound(theGlyph) || !theRegion || !isCacheable(*theRegion))
      {
        return;
      }
      prepareKey(theGlyph);
      myRegions.Bind(theGlyph, Entry{theRegion, {}, {}});
    }

    void BindShape(const Font_GlyphOutline::Glyph& theGlyph,
                   const bool                      theToConcatenateContours,
                   const TopoDS_Shape&             theShape)
    {
      Entry* anEntry = myRegions.ChangeSeek(theGlyph);
      if (anEntry == nullptr)
      {
        return;
      }
      TopoDS_Shape& aCachedShape =
        theToConcatenateContours ? anEntry->CompositeShape : anEntry->DefaultShape;
      if (aCachedShape.IsNull())
      {
        aCachedShape = theShape;
      }
    }

    void Clear()
    {
      myRegions.Clear();
      myKeys.Clear();
      myNextEviction = 0;
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

    void prepareKey(const Font_GlyphOutline::Glyph& theGlyph)
    {
      if (myRegions.IsBound(theGlyph))
      {
        return;
      }
      if (myKeys.Size() < THE_CAPACITY)
      {
        myKeys.Append(theGlyph);
        return;
      }
      myRegions.UnBind(myKeys[myNextEviction]);
      myKeys[myNextEviction] = theGlyph;
      myNextEviction         = (myNextEviction + 1) % THE_CAPACITY;
    }

  private:
    static constexpr size_t THE_CAPACITY              = 256;
    static constexpr size_t THE_MAX_REGION_COMPLEXITY = 16384;
    NCollection_DataMap<Font_GlyphOutline::Glyph, Entry, Font_GlyphOutline::Glyph::Hasher>
                                                       myRegions;
    NCollection_LinearVector<Font_GlyphOutline::Glyph> myKeys;
    size_t                                             myNextEviction = 0;
  };

  void ClearCache()
  {
    std::unique_lock<std::shared_mutex> aLock(CacheMutex);
    Regions.Clear();
  }

  std::optional<Request> MakeRequest(const char32_t theChar)
  {
    std::unique_lock<std::shared_mutex> aLock(FontMutex);
    if (!(Size > 0.0) || !Font->IsValid())
    {
      return std::nullopt;
    }
    const std::optional<Font_GlyphOutline::Glyph> aGlyph = Font->ResolveGlyph(theChar);
    return aGlyph.has_value() ? std::optional<Request>(Request{*aGlyph, Size}) : std::nullopt;
  }

  std::shared_ptr<const BRepFont_PlanarRegion> LoadRegion(const Request& theRequest)
  {
    std::shared_ptr<const BRepFont_PlanarRegion> aCachedRegion;
    {
      std::shared_lock<std::shared_mutex> aLock(CacheMutex);
      aCachedRegion = Regions.Find(theRequest.Glyph);
    }
    if (aCachedRegion)
    {
      std::shared_lock<std::shared_mutex> aFontLock(FontMutex);
      return Font->IsGlyphValid(theRequest.Glyph) ? aCachedRegion
                                                  : std::shared_ptr<const BRepFont_PlanarRegion>();
    }

    std::optional<Font_GlyphOutline> anOutline;
    {
      std::unique_lock<std::shared_mutex> aLock(FontMutex);
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

    std::shared_lock<std::shared_mutex> aFontLock(FontMutex);
    if (!Font->IsGlyphValid(theRequest.Glyph))
    {
      return {};
    }
    std::unique_lock<std::shared_mutex> aCacheLock(CacheMutex);
    if (const std::shared_ptr<const BRepFont_PlanarRegion> aPublished =
          Regions.Find(theRequest.Glyph))
    {
      return aPublished;
    }
    Regions.Bind(theRequest.Glyph, aRegion);
    return aRegion;
  }

  TopoDS_Shape RenderGlyph(const Request& theRequest, const bool theToConcatenateContours)
  {
    TopoDS_Shape aShape;
    {
      std::shared_lock<std::shared_mutex> aLock(CacheMutex);
      aShape = Regions.FindShape(theRequest.Glyph, theToConcatenateContours);
    }
    if (!aShape.IsNull())
    {
      std::shared_lock<std::shared_mutex> aFontLock(FontMutex);
      return Font->IsGlyphValid(theRequest.Glyph) ? aShape : TopoDS_Shape();
    }

    const std::shared_ptr<const BRepFont_PlanarRegion> aRegion = LoadRegion(theRequest);
    if (!aRegion)
    {
      return {};
    }

    BRepFont_Builder::GlyphOptions anOptions;
    anOptions.Size                  = theRequest.Size;
    anOptions.Tolerance             = Tolerance;
    anOptions.ToConcatenateContours = theToConcatenateContours;
    aShape                          = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);

    std::shared_lock<std::shared_mutex> aFontLock(FontMutex);
    if (!Font->IsGlyphValid(theRequest.Glyph))
    {
      return {};
    }
    std::unique_lock<std::shared_mutex> aCacheLock(CacheMutex);
    const TopoDS_Shape                  aPublishedShape =
      Regions.FindShape(theRequest.Glyph, theToConcatenateContours);
    if (!aPublishedShape.IsNull())
    {
      return aPublishedShape;
    }
    Regions.BindShape(theRequest.Glyph, theToConcatenateContours, aShape);
    return aShape;
  }

  std::optional<NCollection_LinearVector<PendingItem>> PrepareText(
    const NCollection_String&               theText,
    const Graphic3d_HorizontalTextAlignment theHorizontalAlignment,
    const Graphic3d_VerticalTextAlignment   theVerticalAlignment)
  {
    NCollection_LinearVector<PendingItem> aPendingItems;
    double                                aSize        = 0.0;
    double                                aMetricScale = 0.0;
    {
      std::unique_lock<std::shared_mutex> aLock(FontMutex);
      if (!(Size > 0.0) || !Font->IsValid())
      {
        return std::nullopt;
      }
      aSize        = Size;
      aMetricScale = MetricScale;

      Font_TextFormatter aFormatter;
      aFormatter.SetupAlignment(theHorizontalAlignment, theVerticalAlignment);
      aFormatter.Append(theText, *Font);
      aFormatter.Format();
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
        aPendingItems.Append(PendingItem{Request{*aGlyph, aSize},
                                         gp_XY(static_cast<double>(aCorner.x()) * aMetricScale,
                                               static_cast<double>(aCorner.y()) * aMetricScale)});
      }
    }
    return !aPendingItems.IsEmpty()
             ? std::optional<NCollection_LinearVector<PendingItem>>(std::move(aPendingItems))
             : std::nullopt;
  }

public:
  occ::handle<Font_FTFont>  Font;
  Cache                     Regions;
  mutable std::shared_mutex FontMutex;
  mutable std::shared_mutex CacheMutex;
  const double              Tolerance;
  double                    MetricScale = 1.0;
  double                    Size        = 0.0;
};

//=================================================================================================

StdPrs_BRepFont::StdPrs_BRepFont()
    : myImpl(std::make_unique<Impl>())
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
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  myImpl->ClearCache();
  myImpl->Font->Release();
  myImpl->MetricScale = 1.0;
  myImpl->Size        = 0.0;
}

//=================================================================================================

bool StdPrs_BRepFont::IsValid() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->Size > 0.0 && myImpl->Font->IsValid();
}

//=================================================================================================

bool StdPrs_BRepFont::Init(const NCollection_String& theFontPath,
                           const double              theSize,
                           const int                 theFaceId)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  if (!std::isfinite(theSize) || !(theSize > myImpl->Tolerance * 100.0))
  {
    return false;
  }
  const Font_FTFont::Params aParameters{72, 4800};
  if (!myImpl->Font->Init(theFontPath.ToCString(), aParameters, theFaceId))
  {
    return false;
  }
  myImpl->ClearCache();
  myImpl->Size        = theSize;
  myImpl->MetricScale = theSize / 4800.0;
  return true;
}

//=================================================================================================

bool StdPrs_BRepFont::FindAndInit(const TCollection_AsciiString& theFontName,
                                  const Font_FontAspect          theFontAspect,
                                  const double                   theSize,
                                  const Font_StrictLevel         theStrictLevel)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  if (!std::isfinite(theSize) || !(theSize > myImpl->Tolerance * 100.0))
  {
    return false;
  }
  const Font_FTFont::Params aParameters{72, 4800};
  if (!myImpl->Font->FindAndInit(theFontName, theFontAspect, aParameters, theStrictLevel))
  {
    return false;
  }
  myImpl->ClearCache();
  myImpl->Size        = theSize;
  myImpl->MetricScale = theSize / 4800.0;
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
  if (!isFinite(theOptions.Pen))
  {
    return {};
  }
  const std::optional<NCollection_LinearVector<Impl::PendingItem>> anItems =
    myImpl->PrepareText(theText, theOptions.HorizontalAlignment, theOptions.VerticalAlignment);
  if (!anItems.has_value())
  {
    return {};
  }

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  for (const Impl::PendingItem& anItem : *anItems)
  {
    TopoDS_Shape aGlyph =
      myImpl->RenderGlyph(anItem.GlyphRequest, theOptions.ToConcatenateContours);
    if (aGlyph.IsNull())
    {
      continue;
    }
    gp_Trsf aPosition;
    aPosition.SetTranslation(gp_Vec(anItem.Position.X(), anItem.Position.Y(), 0.0));
    aGlyph.Move(aPosition);
    aBuilder.Add(aCompound, aGlyph);
  }
  if (aCompound.NbChildren() == 0)
  {
    return {};
  }
  gp_Trsf aPenTransform;
  aPenTransform.SetTransformation(theOptions.Pen, gp_Ax3(gp::XOY()));
  aCompound.Move(aPenTransform);
  return aCompound;
}

//=================================================================================================

bool StdPrs_BRepFont::SetWidthScaling(const float theScaleFactor)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  if (!std::isfinite(theScaleFactor) || !(theScaleFactor > 0.0f))
  {
    return false;
  }
  if (myImpl->Font->WidthScaling() == theScaleFactor)
  {
    return true;
  }
  if (!myImpl->Font->SetWidthScaling(theScaleFactor))
  {
    return false;
  }
  myImpl->ClearCache();
  return true;
}

//=================================================================================================

void StdPrs_BRepFont::SetSingleStrokeFont(const bool theIsSingleStroke)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  if (myImpl->Font->IsSingleStrokeFont() == theIsSingleStroke)
  {
    return;
  }
  myImpl->Font->SetSingleStrokeFont(theIsSingleStroke);
  myImpl->ClearCache();
}

//=================================================================================================

double StdPrs_BRepFont::Ascender() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->Ascender());
}

//=================================================================================================

double StdPrs_BRepFont::Descender() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->Descender());
}

//=================================================================================================

double StdPrs_BRepFont::LineSpacing() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->LineSpacing());
}

//=================================================================================================

double StdPrs_BRepFont::PointSize() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->PointSize());
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceX(const char32_t theUCharNext)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->AdvanceX(theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceX(const char32_t theUChar, const char32_t theUCharNext)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->AdvanceX(theUChar, theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceY(const char32_t theUCharNext)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->AdvanceY(theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::AdvanceY(const char32_t theUChar, const char32_t theUCharNext)
{
  std::unique_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale * static_cast<double>(myImpl->Font->AdvanceY(theUChar, theUCharNext));
}

//=================================================================================================

double StdPrs_BRepFont::Scale() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->MetricScale;
}

//=================================================================================================

double StdPrs_BRepFont::Size() const
{
  std::shared_lock<std::shared_mutex> aLock(myImpl->FontMutex);
  return myImpl->Size;
}
