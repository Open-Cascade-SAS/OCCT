// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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
#include <Font_TextFormatter.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#ifdef HAVE_FREETYPE
  #include <ft2build.h>
  #include FT_FREETYPE_H
  #include FT_OUTLINE_H
#endif

IMPLEMENT_STANDARD_RTTIEXT(Font_FTFont, Standard_Transient)

namespace
{
constexpr size_t subsetIndex(const Font_UnicodeSubset theSubset) noexcept
{
  return static_cast<size_t>(theSubset);
}
} // namespace

#ifdef HAVE_FREETYPE
namespace
{
struct OutlineContext
{
  NCollection_LinearVector<Font_GlyphOutline::Segment>* Segments = nullptr;
  NCollection_LinearVector<Font_GlyphOutline::Contour>* Contours = nullptr;
  gp_XY                                                 Current;
  gp_XY                                                 ContourStart;
  size_t                                                NbSegments      = 0;
  size_t                                                NbContours      = 0;
  uint32_t                                              FirstSegment    = 0;
  double                                                CoordinateScale = 1.0;
  double                                                WidthScale      = 1.0;
  bool                                                  HasContour      = false;
  bool                                                  IsSingleStroke  = false;
};

struct OutlineStorage
{
  uint32_t                                              GlyphIndex;
  double&                                               UnitsPerEm;
  Font_GlyphOutline::FillRule&                          FillRule;
  bool&                                                 IsSingleStroke;
  bool&                                                 HasPossibleOverlaps;
  NCollection_LinearVector<Font_GlyphOutline::Segment>& Segments;
  NCollection_LinearVector<Font_GlyphOutline::Contour>& Contours;
};

int32_t toFTPoints(const unsigned int thePointSize) noexcept
{
  return static_cast<int32_t>(thePointSize) * 64;
}

template <typename theReturn_t, typename theFTUnits_t>
theReturn_t fromFTPoints(const theFTUnits_t theFTUnits) noexcept
{
  return static_cast<theReturn_t>(theFTUnits) / 64.0f;
}

std::optional<uint32_t> outlineIndex(const size_t theValue) noexcept
{
  const uint32_t anIndex = static_cast<uint32_t>(theValue);
  return static_cast<size_t>(anIndex) == theValue ? std::optional<uint32_t>(anIndex) : std::nullopt;
}

gp_XY fromOutlinePoint(const FT_Vector& thePoint, const OutlineContext& theContext) noexcept
{
  return gp_XY(static_cast<double>(thePoint.x) * theContext.CoordinateScale * theContext.WidthScale,
               static_cast<double>(thePoint.y) * theContext.CoordinateScale);
}

bool finishOutlineContour(OutlineContext& theContext) noexcept
{
  if (theContext.Segments == nullptr || theContext.Contours == nullptr)
  {
    return false;
  }
  if (!theContext.HasContour)
  {
    return true;
  }

  if (!theContext.IsSingleStroke
      && (theContext.Current - theContext.ContourStart).SquareModulus() > 0.0)
  {
    if (theContext.NbSegments >= theContext.Segments->Size())
    {
      return false;
    }
    (*theContext.Segments)[theContext.NbSegments++] =
      Font_GlyphOutline::Segment::Line(theContext.Current, theContext.ContourStart);
  }

  if (theContext.FirstSegment > theContext.NbSegments)
  {
    return false;
  }
  const size_t                  aNbSegments      = theContext.NbSegments - theContext.FirstSegment;
  const std::optional<uint32_t> aNbSegmentsIndex = outlineIndex(aNbSegments);
  if (!aNbSegmentsIndex.has_value())
  {
    return false;
  }
  if (aNbSegments > 0)
  {
    if (theContext.NbContours >= theContext.Contours->Size())
    {
      return false;
    }
    (*theContext.Contours)[theContext.NbContours++] =
      Font_GlyphOutline::Contour(theContext.FirstSegment, *aNbSegmentsIndex);
  }
  theContext.HasContour = false;
  return true;
}

int moveToOutline(const FT_Vector* thePoint, void* theUser) noexcept
{
  if (thePoint == nullptr || theUser == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  OutlineContext& aContext = *static_cast<OutlineContext*>(theUser);
  if (aContext.Segments == nullptr || aContext.Contours == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  if (!finishOutlineContour(aContext))
  {
    return FT_Err_Invalid_Outline;
  }
  const std::optional<uint32_t> aFirstSegment = outlineIndex(aContext.NbSegments);
  if (!aFirstSegment.has_value())
  {
    return FT_Err_Array_Too_Large;
  }
  aContext.Current      = fromOutlinePoint(*thePoint, aContext);
  aContext.ContourStart = aContext.Current;
  aContext.FirstSegment = *aFirstSegment;
  aContext.HasContour   = true;
  return FT_Err_Ok;
}

int lineToOutline(const FT_Vector* thePoint, void* theUser) noexcept
{
  if (thePoint == nullptr || theUser == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  OutlineContext& aContext = *static_cast<OutlineContext*>(theUser);
  if (aContext.Segments == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  if (aContext.NbSegments >= aContext.Segments->Size())
  {
    return FT_Err_Invalid_Outline;
  }
  const gp_XY anEnd = fromOutlinePoint(*thePoint, aContext);
  (*aContext.Segments)[aContext.NbSegments++] =
    Font_GlyphOutline::Segment::Line(aContext.Current, anEnd);
  aContext.Current = anEnd;
  return FT_Err_Ok;
}

int conicToOutline(const FT_Vector* theControl, const FT_Vector* thePoint, void* theUser) noexcept
{
  if (theControl == nullptr || thePoint == nullptr || theUser == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  OutlineContext& aContext = *static_cast<OutlineContext*>(theUser);
  if (aContext.Segments == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  if (aContext.NbSegments >= aContext.Segments->Size())
  {
    return FT_Err_Invalid_Outline;
  }
  const gp_XY aControl = fromOutlinePoint(*theControl, aContext);
  const gp_XY anEnd    = fromOutlinePoint(*thePoint, aContext);
  (*aContext.Segments)[aContext.NbSegments++] =
    Font_GlyphOutline::Segment::Quadratic(aContext.Current, aControl, anEnd);
  aContext.Current = anEnd;
  return FT_Err_Ok;
}

int cubicToOutline(const FT_Vector* theControl1,
                   const FT_Vector* theControl2,
                   const FT_Vector* thePoint,
                   void*            theUser) noexcept
{
  if (theControl1 == nullptr || theControl2 == nullptr || thePoint == nullptr || theUser == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  OutlineContext& aContext = *static_cast<OutlineContext*>(theUser);
  if (aContext.Segments == nullptr)
  {
    return FT_Err_Invalid_Argument;
  }
  if (aContext.NbSegments >= aContext.Segments->Size())
  {
    return FT_Err_Invalid_Outline;
  }
  const gp_XY aControl1 = fromOutlinePoint(*theControl1, aContext);
  const gp_XY aControl2 = fromOutlinePoint(*theControl2, aContext);
  const gp_XY anEnd     = fromOutlinePoint(*thePoint, aContext);
  (*aContext.Segments)[aContext.NbSegments++] =
    Font_GlyphOutline::Segment::Cubic(aContext.Current, aControl1, aControl2, anEnd);
  aContext.Current = anEnd;
  return FT_Err_Ok;
}

bool decomposeOutline(FT_Face         theFace,
                      const bool      theIsSingleStroke,
                      const double    theWidthScale,
                      const int32_t   theRegularLoadFlags,
                      OutlineStorage& theStorage)
{
  if (theFace == nullptr || theFace->units_per_EM == 0)
  {
    return false;
  }

  FT_Int32 aLoadFlags       = FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
  double   aCoordinateScale = 1.0;
  if ((theFace->face_flags & FT_FACE_FLAG_TRICKY) != 0)
  {
    aLoadFlags = theRegularLoadFlags | FT_LOAD_NO_BITMAP;
    if (theFace->size == nullptr || theFace->size->metrics.y_ppem == 0)
    {
      return false;
    }
    aCoordinateScale = static_cast<double>(theFace->units_per_EM)
                       / (static_cast<double>(theFace->size->metrics.y_ppem) * 64.0);
  }

  if (FT_Load_Glyph(theFace, static_cast<FT_UInt>(theStorage.GlyphIndex), aLoadFlags) != 0
      || theFace->glyph == nullptr || theFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
  {
    return false;
  }

  FT_Outline& anOutline = theFace->glyph->outline;
  if (FT_Outline_Check(&anOutline) != 0)
  {
    return false;
  }

  theStorage.UnitsPerEm     = static_cast<double>(theFace->units_per_EM);
  theStorage.FillRule       = (anOutline.flags & FT_OUTLINE_EVEN_ODD_FILL) != 0
                                ? Font_GlyphOutline::FillRule::EvenOdd
                                : Font_GlyphOutline::FillRule::NonZero;
  theStorage.IsSingleStroke = theIsSingleStroke;
  #ifdef FT_OUTLINE_OVERLAP
  theStorage.HasPossibleOverlaps = (anOutline.flags & FT_OUTLINE_OVERLAP) != 0;
  #endif
  if (anOutline.n_points > 0)
  {
    const size_t aNbContours =
      anOutline.n_contours > 0 ? static_cast<size_t>(anOutline.n_contours) : 0;
    theStorage.Segments.Resize(static_cast<size_t>(anOutline.n_points) + aNbContours);
  }
  if (anOutline.n_contours > 0)
  {
    theStorage.Contours.Resize(static_cast<size_t>(anOutline.n_contours));
  }

  OutlineContext aContext;
  aContext.Segments        = &theStorage.Segments;
  aContext.Contours        = &theStorage.Contours;
  aContext.CoordinateScale = aCoordinateScale;
  aContext.WidthScale      = theWidthScale;
  aContext.IsSingleStroke  = theIsSingleStroke;

  FT_Outline_Funcs aFunctions;
  aFunctions.move_to  = moveToOutline;
  aFunctions.line_to  = lineToOutline;
  aFunctions.conic_to = conicToOutline;
  aFunctions.cubic_to = cubicToOutline;
  aFunctions.shift    = 0;
  aFunctions.delta    = 0;

  if (FT_Outline_Decompose(&anOutline, &aFunctions, &aContext) != 0
      || !finishOutlineContour(aContext))
  {
    return false;
  }
  theStorage.Segments.Resize(aContext.NbSegments);
  theStorage.Contours.Resize(aContext.NbContours);
  return true;
}
} // namespace
#endif

//=================================================================================================

Font_FTFont::Font_FTFont(const occ::handle<Font_FTLibrary>& theFTLib)
    : myFTLib(theFTLib),
      myFTFace(nullptr),
      myFontAspect(Font_FontAspect_Regular),
      myWidthScaling(1.0),
#ifdef HAVE_FREETYPE
      myLoadFlags(FT_LOAD_NO_HINTING | FT_LOAD_TARGET_NORMAL),
#else
      myLoadFlags(0),
#endif
      myIdentity(std::make_shared<Font_GlyphOutline::Glyph::Identity>()),
      myToUseUnicodeSubsetFallback(Font_FontMgr::ToUseUnicodeSubsetFallback())
{
  if (myFTLib.IsNull())
  {
    myFTLib = new Font_FTLibrary();
  }
}

//=================================================================================================

Font_FTFont::~Font_FTFont()
{
  Release();
}

//=================================================================================================

void Font_FTFont::invalidateGlyphState()
{
  ++myRevision;
  myGlyphImg.Clear();
  myLoadedGlyph.Reset();
}

//=================================================================================================

void Font_FTFont::Release()
{
  invalidateGlyphState();
  for (occ::handle<Font_FTFont>& aFallbackFace : myFallbackFaces)
  {
    aFallbackFace.Nullify();
  }
  myFontPath.Clear();
  if (myFTFace != nullptr)
  {
#ifdef HAVE_FREETYPE
    FT_Done_Face(myFTFace);
#endif
    myFTFace = nullptr;
  }
  myBuffer.Nullify();
}

//=================================================================================================

bool Font_FTFont::Init(const occ::handle<NCollection_Buffer>& theData,
                       const TCollection_AsciiString&         theFileName,
                       const Params&                          theParams,
                       const int                              theFaceId)
{
  if (!theParams.IsValid() || !myFTLib->IsValid())
  {
    if (!myFTLib->IsValid())
    {
      Message::SendTrace("FreeType library is unavailable");
    }
    return false;
  }

#ifdef HAVE_FREETYPE
  TCollection_AsciiString aNewPath(theFileName);
  int32_t                 aLoadFlags = FT_LOAD_TARGET_NORMAL;
  if ((theParams.FontHinting & Font_Hinting_Light) != 0)
  {
    aLoadFlags = FT_LOAD_TARGET_LIGHT;
  }
  else if ((theParams.FontHinting & Font_Hinting_Normal) == 0)
  {
    aLoadFlags |= FT_LOAD_NO_HINTING;
  }
  if ((theParams.FontHinting & Font_Hinting_ForceAutohint) != 0)
  {
    aLoadFlags |= FT_LOAD_FORCE_AUTOHINT;
  }
  if ((theParams.FontHinting & Font_Hinting_NoAutohint) != 0)
  {
    aLoadFlags |= FT_LOAD_NO_AUTOHINT;
  }

  FT_Face aNewFace = nullptr;
  if (!theData.IsNull())
  {
    if (FT_New_Memory_Face(myFTLib->Instance(),
                           theData->Data(),
                           static_cast<FT_Long>(theData->Size()),
                           static_cast<FT_Long>(theFaceId),
                           &aNewFace)
        != 0)
    {
      Message::SendTrace(TCollection_AsciiString("Font '") + aNewPath
                         + "' failed to load from memory");
      return false;
    }
  }
  else
  {
    if (FT_New_Face(myFTLib->Instance(),
                    aNewPath.ToCString(),
                    static_cast<FT_Long>(theFaceId),
                    &aNewFace)
        != 0)
    {
      return false;
    }
  }
  std::unique_ptr<FT_FaceRec_, decltype(&FT_Done_Face)> aNewFaceOwner(aNewFace, &FT_Done_Face);

  if (FT_Select_Charmap(aNewFaceOwner.get(), ft_encoding_unicode) != 0)
  {
    Message::SendTrace(TCollection_AsciiString("Font '") + aNewPath
                       + "' doesn't contains Unicode charmap");
    return false;
  }
  if (FT_Set_Char_Size(aNewFaceOwner.get(),
                       0L,
                       toFTPoints(theParams.PointSize),
                       theParams.Resolution,
                       theParams.Resolution)
      != 0)
  {
    Message::SendTrace(TCollection_AsciiString("Font '") + aNewPath
                       + "' doesn't contains Unicode charmap of requested size");
    return false;
  }

  if (theParams.ToSynthesizeItalic)
  {
    const double THE_SHEAR_ANGLE = 10.0 * M_PI / 180.0;

    FT_Matrix aMat;
    aMat.xx = static_cast<FT_Fixed>(std::cos(-THE_SHEAR_ANGLE) * (1 << 16));
    aMat.xy = 0;
    aMat.yx = 0;
    aMat.yy = aMat.xx;

    const FT_Fixed aFactor = static_cast<FT_Fixed>(std::tan(THE_SHEAR_ANGLE) * (1 << 16));
    aMat.xy += FT_MulFix(aFactor, aMat.xx);

    FT_Set_Transform(aNewFaceOwner.get(), &aMat, nullptr);
  }

  Release();
  myBuffer     = theData;
  myFontPath   = std::move(aNewPath);
  myFontParams = theParams;
  myLoadFlags  = aLoadFlags;
  myFTFace     = aNewFaceOwner.release();
  return true;
#else
  (void)theData;
  (void)theFileName;
  (void)theParams;
  (void)theFaceId;
  return false;
#endif
}

//=================================================================================================

occ::handle<Font_FTFont> Font_FTFont::FindAndCreate(const TCollection_AsciiString& theFontName,
                                                    const Font_FontAspect          theFontAspect,
                                                    const Params&                  theParams,
                                                    const Font_StrictLevel         theStrictLevel)
{
  occ::handle<Font_FTFont> aFont = new Font_FTFont();
  return aFont->FindAndInit(theFontName, theFontAspect, theParams, theStrictLevel)
           ? aFont
           : occ::handle<Font_FTFont>();
}

//=================================================================================================

bool Font_FTFont::FindAndInit(const TCollection_AsciiString& theFontName,
                              Font_FontAspect                theFontAspect,
                              const Params&                  theParams,
                              Font_StrictLevel               theStrictLevel)
{
  Params                             aParams  = theParams;
  const occ::handle<Font_FontMgr>    aFontMgr = Font_FontMgr::GetInstance();
  Font_FontAspect                    anAspect = theFontAspect;
  const occ::handle<Font_SystemFont> aSystemFont =
    aFontMgr->FindFont(theFontName, theStrictLevel, anAspect);
  if (!aSystemFont.IsNull())
  {
    aParams.IsSingleStrokeFont =
      aParams.IsSingleStrokeFont || aSystemFont->IsSingleStrokeFont();
    int                           aFaceId = 0;
    const TCollection_AsciiString aPath =
      aSystemFont->FontPathAny(anAspect, aParams.ToSynthesizeItalic, aFaceId);
    if (aPath.IsEmpty() || !Init(aPath, aParams, aFaceId))
    {
      return false;
    }
    myFontAspect = anAspect;
    return true;
  }
#ifdef HAVE_FREETYPE
  else if (theStrictLevel == Font_StrictLevel_Any)
  {
    Font_FontAspect aResolvedAspect = Font_FontAspect_Regular;
    switch (theFontAspect)
    {
      case Font_FontAspect_UNDEFINED:
      case Font_FontAspect_Regular:
      case Font_FontAspect_Bold:
        break;
      case Font_FontAspect_Italic:
      case Font_FontAspect_BoldItalic:
        aResolvedAspect            = Font_FontAspect_Italic;
        aParams.ToSynthesizeItalic = true;
        break;
    }
    if (!Init(Font_FontMgr::EmbedFallbackFont(), "Embed Fallback Font", aParams, 0))
    {
      return false;
    }
    myFontAspect = aResolvedAspect;
    return true;
  }
#endif
  return false;
}

//=================================================================================================

bool Font_FTFont::findAndInitFallback(Font_UnicodeSubset theSubset)
{
  const size_t aSubsetIndex = subsetIndex(theSubset);
  if (aSubsetIndex >= myFallbackFaces.size())
  {
    return false;
  }
  if (!myFallbackFaces[aSubsetIndex].IsNull())
  {
    return myFallbackFaces[aSubsetIndex]->IsValid();
  }

#ifdef HAVE_FREETYPE
  myFallbackFaces[aSubsetIndex]                               = new Font_FTFont(myFTLib);
  myFallbackFaces[aSubsetIndex]->myToUseUnicodeSubsetFallback = false; // no recursion

  const occ::handle<Font_FontMgr>    aFontMgr = Font_FontMgr::GetInstance();
  const occ::handle<Font_SystemFont> aSystemFont =
    aFontMgr->FindFallbackFont(theSubset, myFontAspect);
  if (!aSystemFont.IsNull())
  {
    Params          aParams    = myFontParams;
    Font_FontAspect anAspect   = myFontAspect;
    int             aFaceId    = 0;
    aParams.IsSingleStrokeFont =
      aParams.IsSingleStrokeFont || aSystemFont->IsSingleStrokeFont();
    const TCollection_AsciiString aPath =
      aSystemFont->FontPathAny(anAspect, aParams.ToSynthesizeItalic, aFaceId);
    if (!aPath.IsEmpty() && myFallbackFaces[aSubsetIndex]->Init(aPath, aParams, aFaceId))
    {
      myFallbackFaces[aSubsetIndex]->myFontAspect = anAspect;
      const char* aPrimaryName =
        myFTFace->family_name != nullptr ? myFTFace->family_name : myFontPath.ToCString();
      Message::SendTrace(TCollection_AsciiString("Font_FTFont, using fallback font '")
                         + aSystemFont->FontName() + "'" + " for symbols unsupported by '"
                         + aPrimaryName + "'");
    }
  }
#endif
  return !myFallbackFaces[aSubsetIndex].IsNull() && myFallbackFaces[aSubsetIndex]->IsValid();
}

//=================================================================================================

bool Font_FTFont::HasSymbol(char32_t theUChar) const
{
#ifdef HAVE_FREETYPE
  return myFTFace != nullptr && FT_Get_Char_Index(myFTFace, theUChar) != 0;
#else
  (void)theUChar;
  return false;
#endif
}

//=================================================================================================

bool Font_FTFont::SetWidthScaling(const float theScaleFactor)
{
  if (!std::isfinite(theScaleFactor) || !(theScaleFactor > 0.0f))
  {
    return false;
  }
  if (myWidthScaling == theScaleFactor)
  {
    return true;
  }
  myWidthScaling = theScaleFactor;
  invalidateGlyphState();
  return true;
}

//=================================================================================================

void Font_FTFont::SetSingleStrokeFont(const bool theIsSingleLine)
{
  if (myFontParams.IsSingleStrokeFont == theIsSingleLine)
  {
    return;
  }
  for (occ::handle<Font_FTFont>& aFallbackFace : myFallbackFaces)
  {
    aFallbackFace.Nullify();
  }
  myFontParams.IsSingleStrokeFont = theIsSingleLine;
  invalidateGlyphState();
}

//=================================================================================================

void Font_FTFont::SetUseUnicodeSubsetFallback(const bool theToFallback)
{
  if (myToUseUnicodeSubsetFallback == theToFallback)
  {
    return;
  }
  myToUseUnicodeSubsetFallback = theToFallback;
  invalidateGlyphState();
}

//=================================================================================================

std::optional<Font_FTFont::GlyphSelection> Font_FTFont::selectGlyph(const char32_t theUChar)
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr || theUChar == 0)
  {
    return std::nullopt;
  }

  const FT_UInt aPrimaryGlyph = FT_Get_Char_Index(myFTFace, theUChar);
  if (aPrimaryGlyph != 0)
  {
    const std::optional<uint32_t> aGlyphIndex = outlineIndex(static_cast<size_t>(aPrimaryGlyph));
    if (!aGlyphIndex.has_value())
    {
      return std::nullopt;
    }
    return GlyphSelection(this, 0, *aGlyphIndex);
  }

  if (myToUseUnicodeSubsetFallback)
  {
    const Font_UnicodeSubset aSubset      = CharSubset(theUChar);
    const size_t             aSubsetIndex = subsetIndex(aSubset);
    if (findAndInitFallback(aSubset) && myFallbackFaces[aSubsetIndex]->HasSymbol(theUChar))
    {
      const FT_UInt aFallbackGlyph =
        FT_Get_Char_Index(myFallbackFaces[aSubsetIndex]->myFTFace, theUChar);
      const std::optional<uint32_t> aGlyphIndex = outlineIndex(static_cast<size_t>(aFallbackGlyph));
      if (!aGlyphIndex.has_value())
      {
        return std::nullopt;
      }
      return GlyphSelection(myFallbackFaces[aSubsetIndex].get(),
                            static_cast<uint32_t>(aSubset) + 1,
                            *aGlyphIndex);
    }
  }

  // Preserve the established missing-character behavior by selecting .notdef from the primary face.
  return GlyphSelection(this, 0, 0);
#else
  (void)theUChar;
  return std::nullopt;
#endif
}

//=================================================================================================

std::optional<Font_GlyphOutline::Glyph> Font_FTFont::ResolveGlyph(const char32_t theUChar)
{
  const std::optional<GlyphSelection> aSelection = selectGlyph(theUChar);
  return aSelection.has_value()
           ? std::optional<Font_GlyphOutline::Glyph>(Font_GlyphOutline::Glyph(myIdentity,
                                                                              myRevision,
                                                                              aSelection->Face,
                                                                              aSelection->Index))
           : std::nullopt;
}

//=================================================================================================

std::optional<Font_GlyphOutline> Font_FTFont::LoadGlyphOutline(const char32_t theUChar)
{
  const std::optional<Font_GlyphOutline::Glyph> aGlyph = ResolveGlyph(theUChar);
  if (!aGlyph.has_value())
  {
    return std::nullopt;
  }
  return LoadGlyphOutline(*aGlyph);
}

//=================================================================================================

bool Font_FTFont::IsGlyphValid(const Font_GlyphOutline::Glyph& theGlyph) const
{
#ifdef HAVE_FREETYPE
  const Font_FTFont* aFont = fontForGlyph(theGlyph);
  return aFont != nullptr && aFont->myFTFace != nullptr
         && static_cast<uint64_t>(theGlyph.Index())
              < static_cast<uint64_t>(aFont->myFTFace->num_glyphs);
#else
  (void)theGlyph;
  return false;
#endif
}

//=================================================================================================

const Font_FTFont* Font_FTFont::fontForGlyph(const Font_GlyphOutline::Glyph& theGlyph) const
{
  if (theGlyph.Owner() != myIdentity || theGlyph.Revision() != myRevision)
  {
    return nullptr;
  }
  if (theGlyph.Face() == 0)
  {
    return this;
  }
  const uint32_t aFallbackIndex = theGlyph.Face() - 1;
  return aFallbackIndex < static_cast<uint32_t>(Font_UnicodeSubset_NB)
             && !myFallbackFaces[aFallbackIndex].IsNull()
           ? myFallbackFaces[aFallbackIndex].get()
           : nullptr;
}

//=================================================================================================

std::optional<Font_GlyphOutline> Font_FTFont::LoadGlyphOutline(
  const Font_GlyphOutline::Glyph& theGlyph)
{
  myGlyphImg.Clear();
  myLoadedGlyph.Reset();
#ifdef HAVE_FREETYPE
  const Font_FTFont* aFont = fontForGlyph(theGlyph);
  if (aFont == nullptr || aFont->myFTFace == nullptr || aFont->myFTFace->units_per_EM == 0
      || static_cast<uint64_t>(theGlyph.Index())
           >= static_cast<uint64_t>(aFont->myFTFace->num_glyphs))
  {
    return std::nullopt;
  }

  Font_GlyphOutline aResult;
  OutlineStorage    aStorage{theGlyph.Index(),
                             aResult.myUnitsPerEm,
                             aResult.myFillRule,
                             aResult.myIsSingleStroke,
                             aResult.myHasPossibleOverlaps,
                             aResult.mySegments,
                             aResult.myContours};
  if (!decomposeOutline(aFont->myFTFace,
                        aFont->myFontParams.IsSingleStrokeFont,
                        static_cast<double>(myWidthScaling),
                        static_cast<FT_Int32>(aFont->myLoadFlags),
                        aStorage))
  {
    return std::nullopt;
  }
  return aResult;
#else
  (void)theGlyph;
  return std::nullopt;
#endif
}

//=================================================================================================

bool Font_FTFont::loadGlyph(const char32_t theUChar)
{
  if (myLoadedGlyph.Character == theUChar)
  {
    return myLoadedGlyph.Face != nullptr;
  }

  myGlyphImg.Clear();
  myLoadedGlyph.Reset();
#ifdef HAVE_FREETYPE
  const std::optional<GlyphSelection> aSelection = selectGlyph(theUChar);
  if (!aSelection.has_value())
  {
    return false;
  }

  if (FT_Load_Glyph(aSelection->Provider->myFTFace,
                    static_cast<FT_UInt>(aSelection->Index),
                    static_cast<FT_Int32>(aSelection->Provider->myLoadFlags))
        != 0
      || aSelection->Provider->myFTFace->glyph == nullptr)
  {
    return false;
  }

  myLoadedGlyph.Face      = aSelection->Provider->myFTFace;
  myLoadedGlyph.Character = theUChar;
  return true;
#else
  (void)theUChar;
  return false;
#endif
}

//=================================================================================================

bool Font_FTFont::RenderGlyph(const char32_t theUChar)
{
  myGlyphImg.Clear();
  myLoadedGlyph.Reset();

#ifdef HAVE_FREETYPE
  const std::optional<GlyphSelection> aSelection = selectGlyph(theUChar);
  if (!aSelection.has_value())
  {
    return false;
  }

  if (FT_Load_Glyph(aSelection->Provider->myFTFace,
                    static_cast<FT_UInt>(aSelection->Index),
                    static_cast<FT_Int32>(aSelection->Provider->myLoadFlags | FT_LOAD_RENDER))
        != 0
      || aSelection->Provider->myFTFace->glyph == nullptr
      || aSelection->Provider->myFTFace->glyph->format != FT_GLYPH_FORMAT_BITMAP)
  {
    return false;
  }

  const FT_Bitmap& aBitmap = aSelection->Provider->myFTFace->glyph->bitmap;
  if (aBitmap.buffer == nullptr || aBitmap.width == 0 || aBitmap.rows == 0)
  {
    return false;
  }

  if (aBitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
  {
    if (!myGlyphImg.InitWrapper(Image_Format_Alpha,
                                aBitmap.buffer,
                                aBitmap.width,
                                aBitmap.rows,
                                std::abs(aBitmap.pitch)))
    {
      return false;
    }
    myGlyphImg.SetTopDown(aBitmap.pitch > 0);
  }
  else if (aBitmap.pixel_mode == FT_PIXEL_MODE_MONO)
  {
    if (!myGlyphImg.InitTrash(Image_Format_Gray, aBitmap.width, aBitmap.rows))
    {
      return false;
    }

    myGlyphImg.SetTopDown(aBitmap.pitch > 0);
    const int aNumOfBytesInRow = aBitmap.width / 8 + (aBitmap.width % 8 ? 1 : 0);
    for (int aRow = 0; aRow < static_cast<int>(aBitmap.rows); ++aRow)
    {
      for (int aCol = 0; aCol < static_cast<int>(aBitmap.width); ++aCol)
      {
        const int aBitOn =
          aBitmap.buffer[aNumOfBytesInRow * aRow + aCol / 8] & (0x80 >> (aCol % 8));
        *myGlyphImg.ChangeRawValue(aRow, aCol) = aBitOn ? 255 : 0;
      }
    }
  }
  else
  {
    return false;
  }

  myLoadedGlyph.Face      = aSelection->Provider->myFTFace;
  myLoadedGlyph.Character = theUChar;
  myLoadedGlyph.HasBitmap = true;
  return true;
#else
  (void)theUChar;
  return false;
#endif
}

//=================================================================================================

unsigned int Font_FTFont::GlyphMaxSizeX(bool theToIncludeFallback) const
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr || myFTFace->size == nullptr)
  {
    return 0;
  }
  if (!theToIncludeFallback)
  {
    float aWidth = (FT_IS_SCALABLE(myFTFace) != 0 && myFTFace->units_per_EM != 0)
                     ? static_cast<float>(myFTFace->bbox.xMax - myFTFace->bbox.xMin)
                         * (static_cast<float>(myFTFace->size->metrics.x_ppem)
                            / static_cast<float>(myFTFace->units_per_EM))
                     : fromFTPoints<float>(myFTFace->size->metrics.max_advance);
    return (unsigned int)(aWidth + 0.5f);
  }

  unsigned int aWidth = GlyphMaxSizeX(false);
  if (theToIncludeFallback)
  {
    for (const occ::handle<Font_FTFont>& aFallback : myFallbackFaces)
    {
      if (!aFallback.IsNull() && aFallback->IsValid())
      {
        aWidth = std::max(aWidth, aFallback->GlyphMaxSizeX(false));
      }
    }
  }
  return aWidth;
#else
  (void)theToIncludeFallback;
  return 0;
#endif
}

//=================================================================================================

unsigned int Font_FTFont::GlyphMaxSizeY(bool theToIncludeFallback) const
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr || myFTFace->size == nullptr)
  {
    return 0;
  }
  if (!theToIncludeFallback)
  {
    float aHeight = (FT_IS_SCALABLE(myFTFace) != 0 && myFTFace->units_per_EM != 0)
                      ? static_cast<float>(myFTFace->bbox.yMax - myFTFace->bbox.yMin)
                          * (static_cast<float>(myFTFace->size->metrics.y_ppem)
                             / static_cast<float>(myFTFace->units_per_EM))
                      : fromFTPoints<float>(myFTFace->size->metrics.height);
    return (unsigned int)(aHeight + 0.5f);
  }

  unsigned int aHeight = GlyphMaxSizeY(false);
  if (theToIncludeFallback)
  {
    for (const occ::handle<Font_FTFont>& aFallback : myFallbackFaces)
    {
      if (!aFallback.IsNull() && aFallback->IsValid())
      {
        aHeight = std::max(aHeight, aFallback->GlyphMaxSizeY(false));
      }
    }
  }
  return aHeight;
#else
  (void)theToIncludeFallback;
  return 0;
#endif
}

//=================================================================================================

float Font_FTFont::Ascender() const
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr || myFTFace->size == nullptr)
  {
    return 0.0f;
  }
  if (myFTFace->units_per_EM == 0)
  {
    return fromFTPoints<float>(myFTFace->size->metrics.ascender);
  }
  return static_cast<float>(myFTFace->ascender)
         * (static_cast<float>(myFTFace->size->metrics.y_ppem)
            / static_cast<float>(myFTFace->units_per_EM));
#else
  return 0.0f;
#endif
}

//=================================================================================================

float Font_FTFont::Descender() const
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr || myFTFace->size == nullptr)
  {
    return 0.0f;
  }
  if (myFTFace->units_per_EM == 0)
  {
    return fromFTPoints<float>(myFTFace->size->metrics.descender);
  }
  return static_cast<float>(myFTFace->descender)
         * (static_cast<float>(myFTFace->size->metrics.y_ppem)
            / static_cast<float>(myFTFace->units_per_EM));
#else
  return 0.0f;
#endif
}

//=================================================================================================

float Font_FTFont::LineSpacing() const
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr || myFTFace->size == nullptr)
  {
    return 0.0f;
  }
  if (myFTFace->units_per_EM == 0)
  {
    return fromFTPoints<float>(myFTFace->size->metrics.height);
  }
  return static_cast<float>(myFTFace->height)
         * (static_cast<float>(myFTFace->size->metrics.y_ppem)
            / static_cast<float>(myFTFace->units_per_EM));
#else
  return 0.0f;
#endif
}

//=================================================================================================

float Font_FTFont::AdvanceX(char32_t theUChar, char32_t theUCharNext)
{
  loadGlyph(theUChar);
  return AdvanceX(theUCharNext);
}

//=================================================================================================

float Font_FTFont::AdvanceY(char32_t theUChar, char32_t theUCharNext)
{
  loadGlyph(theUChar);
  return AdvanceY(theUCharNext);
}

//=================================================================================================

bool Font_FTFont::getKerning(FT_Vector& theKern, char32_t theUCharCurr, char32_t theUCharNext) const
{
#ifdef HAVE_FREETYPE
  theKern.x = 0;
  theKern.y = 0;
  if (theUCharNext != 0 && myLoadedGlyph.Face != nullptr && FT_HAS_KERNING(myLoadedGlyph.Face) != 0)
  {
    FT_Face aNextFace = nullptr;
    if (myFTFace != nullptr && FT_Get_Char_Index(myFTFace, theUCharNext) != 0)
    {
      aNextFace = myFTFace;
    }
    else if (myToUseUnicodeSubsetFallback)
    {
      const size_t aSubsetIndex = subsetIndex(CharSubset(theUCharNext));
      if (!myFallbackFaces[aSubsetIndex].IsNull()
          && myFallbackFaces[aSubsetIndex]->myFTFace != nullptr
          && FT_Get_Char_Index(myFallbackFaces[aSubsetIndex]->myFTFace, theUCharNext) != 0)
      {
        aNextFace = myFallbackFaces[aSubsetIndex]->myFTFace;
      }
    }
    if (aNextFace != myLoadedGlyph.Face)
    {
      return false;
    }

    const FT_UInt aCharCurr = FT_Get_Char_Index(myLoadedGlyph.Face, theUCharCurr);
    const FT_UInt aCharNext = FT_Get_Char_Index(aNextFace, theUCharNext);
    if (aCharCurr == 0 || aCharNext == 0
        || FT_Get_Kerning(myLoadedGlyph.Face, aCharCurr, aCharNext, FT_KERNING_UNFITTED, &theKern)
             != 0)
    {
      theKern.x = 0;
      theKern.y = 0;
      return false;
    }
    return true;
  }
#else
  (void)theKern;
  (void)theUCharCurr;
  (void)theUCharNext;
#endif
  return false;
}

//=================================================================================================

float Font_FTFont::AdvanceX(char32_t theUCharNext) const
{
  if (myLoadedGlyph.Character == 0)
  {
    return 0.0f;
  }

#ifdef HAVE_FREETYPE
  if (myLoadedGlyph.Face == nullptr || myLoadedGlyph.Face->glyph == nullptr)
  {
    return 0.0f;
  }
  FT_Vector aKern;
  getKerning(aKern, myLoadedGlyph.Character, theUCharNext);
  return myWidthScaling
         * fromFTPoints<float>(myLoadedGlyph.Face->glyph->advance.x + aKern.x
                               + myLoadedGlyph.Face->glyph->lsb_delta
                               - myLoadedGlyph.Face->glyph->rsb_delta);
#else
  (void)theUCharNext;
  return 0.0f;
#endif
}

//=================================================================================================

float Font_FTFont::AdvanceY(char32_t theUCharNext) const
{
  if (myLoadedGlyph.Character == 0)
  {
    return 0.0f;
  }

#ifdef HAVE_FREETYPE
  if (myLoadedGlyph.Face == nullptr || myLoadedGlyph.Face->glyph == nullptr)
  {
    return 0.0f;
  }
  FT_Vector aKern;
  getKerning(aKern, myLoadedGlyph.Character, theUCharNext);
  return fromFTPoints<float>(myLoadedGlyph.Face->glyph->advance.y + aKern.y);
#else
  (void)theUCharNext;
  return 0.0f;
#endif
}

//=================================================================================================

int Font_FTFont::GlyphsNumber(bool theToIncludeFallback) const
{
#ifdef HAVE_FREETYPE
  if (myFTFace == nullptr)
  {
    return 0;
  }
  int aNbGlyphs = static_cast<int>(myFTFace->num_glyphs);
  if (theToIncludeFallback)
  {
    for (const occ::handle<Font_FTFont>& aFallback : myFallbackFaces)
    {
      if (!aFallback.IsNull() && aFallback->IsValid())
      {
        aNbGlyphs += aFallback->GlyphsNumber(false);
      }
    }
  }
  return aNbGlyphs;
#else
  (void)theToIncludeFallback;
  return 0;
#endif
}

//=================================================================================================

void Font_FTFont::GlyphRect(Font_Rect& theRect) const
{
  theRect.Left = theRect.Right = theRect.Top = theRect.Bottom = 0.0f;
#ifdef HAVE_FREETYPE
  if (!myLoadedGlyph.HasBitmap || myLoadedGlyph.Face == nullptr
      || myLoadedGlyph.Face->glyph == nullptr)
  {
    return;
  }
  const FT_Bitmap& aBitmap = myLoadedGlyph.Face->glyph->bitmap;
  theRect.Left             = static_cast<float>(myLoadedGlyph.Face->glyph->bitmap_left);
  theRect.Top              = static_cast<float>(myLoadedGlyph.Face->glyph->bitmap_top);
  theRect.Right =
    static_cast<float>(myLoadedGlyph.Face->glyph->bitmap_left + static_cast<int>(aBitmap.width));
  theRect.Bottom =
    static_cast<float>(myLoadedGlyph.Face->glyph->bitmap_top - static_cast<int>(aBitmap.rows));
#endif
}

//=================================================================================================

Font_Rect Font_FTFont::BoundingBox(const NCollection_String&               theString,
                                   const Graphic3d_HorizontalTextAlignment theAlignX,
                                   const Graphic3d_VerticalTextAlignment   theAlignY)
{
  Font_TextFormatter aFormatter;
  aFormatter.SetupAlignment(theAlignX, theAlignY);
  aFormatter.Reset();

  aFormatter.Append(theString, *this);
  aFormatter.Format();

  Font_Rect aBndBox;
  aFormatter.BndBox(aBndBox);
  return aBndBox;
}
