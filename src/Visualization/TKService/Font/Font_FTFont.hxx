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

#ifndef Font_FTFont_HeaderFile
#define Font_FTFont_HeaderFile

#include <Font_FontAspect.hxx>
#include <Font_GlyphOutline.hxx>
#include <Font_Hinting.hxx>
#include <Font_Rect.hxx>
#include <Font_StrictLevel.hxx>
#include <Font_UnicodeSubset.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Image_PixMap.hxx>
#include <NCollection_String.hxx>
#include <TCollection_AsciiString.hxx>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>

// forward declarations to avoid including of FreeType headers
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_Vector_   FT_Vector;
class Font_FTLibrary;

//! Wrapper over FreeType font.
//! Notice that this class uses internal buffers for loaded glyphs
//! and it is absolutely UNSAFE to load/read glyph from concurrent threads!
class Font_FTFont : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Font_FTFont, Standard_Transient)
public:
  //! Font initialization parameters.
  struct Params
  {
    //! Return whether sizing and hinting policies form a usable configuration.
    [[nodiscard]] bool IsValid() const noexcept
    {
      const bool hasConflictingHinting =
        (FontHinting & Font_Hinting_Light) != 0 && (FontHinting & Font_Hinting_Normal) != 0;
      const bool hasConflictingAutoHinting = (FontHinting & Font_Hinting_ForceAutohint) != 0
                                             && (FontHinting & Font_Hinting_NoAutohint) != 0;
      return PointSize > 0
             && PointSize <= static_cast<unsigned int>(std::numeric_limits<int32_t>::max() / 64)
             && Resolution > 0 && !hasConflictingHinting && !hasConflictingAutoHinting;
    }

    unsigned int PointSize          = 0;                //!< face size in points (1/72 inch)
    unsigned int Resolution         = 72;               //!< target-device resolution in dpi
    Font_Hinting FontHinting        = Font_Hinting_Off; //!< FreeType hinting policy
    bool         ToSynthesizeItalic = false; //!< synthesize italic when the face has no style
    bool         IsSingleStrokeFont = false; //!< interpret outlines as one-line strokes
  };

  //! Find and initialize a font.
  //! @param theFontName    the font name
  //! @param theFontAspect  the font style
  //! @param theParams      initialization parameters
  //! @param theStrictLevel search strict level for using aliases and fallback
  //! @return initialized font, or null when resolution or initialization fails
  Standard_EXPORT static occ::handle<Font_FTFont> FindAndCreate(
    const TCollection_AsciiString& theFontName,
    const Font_FontAspect          theFontAspect,
    const Params&                  theParams,
    const Font_StrictLevel         theStrictLevel = Font_StrictLevel_Any);

  //! Return TRUE if specified character is within subset of modern CJK characters.
  static bool IsCharFromCJK(char32_t theUChar)
  {
    return (theUChar >= 0x03400 && theUChar <= 0x04DFF)
           || (theUChar >= 0x04E00 && theUChar <= 0x09FFF)
           || (theUChar >= 0x0F900 && theUChar <= 0x0FAFF)
           || (theUChar >= 0x20000 && theUChar <= 0x2A6DF)
           || (theUChar >= 0x2F800 && theUChar <= 0x2FA1F)
           // Hiragana and Katakana (Japanese) are NOT part of CJK, but CJK fonts usually include
           // these symbols
           || IsCharFromHiragana(theUChar) || IsCharFromKatakana(theUChar);
  }

  //! Return TRUE if specified character is within subset of Hiragana (Japanese).
  static bool IsCharFromHiragana(char32_t theUChar)
  {
    return (theUChar >= 0x03040 && theUChar <= 0x0309F);
  }

  //! Return TRUE if specified character is within subset of Katakana (Japanese).
  static bool IsCharFromKatakana(char32_t theUChar)
  {
    return (theUChar >= 0x030A0 && theUChar <= 0x030FF);
  }

  //! Return TRUE if specified character is within subset of modern Korean characters (Hangul).
  static bool IsCharFromKorean(char32_t theUChar)
  {
    return (theUChar >= 0x01100 && theUChar <= 0x011FF)
           || (theUChar >= 0x03130 && theUChar <= 0x0318F)
           || (theUChar >= 0x0AC00 && theUChar <= 0x0D7A3);
  }

  //! Return TRUE if specified character is within subset of Arabic characters.
  static bool IsCharFromArabic(char32_t theUChar)
  {
    return (theUChar >= 0x00600 && theUChar <= 0x006FF);
  }

  //! Return TRUE if specified character should be displayed in Right-to-Left order.
  static bool IsCharRightToLeft(char32_t theUChar) { return IsCharFromArabic(theUChar); }

  //! Determine Unicode subset for specified character
  static Font_UnicodeSubset CharSubset(char32_t theUChar)
  {
    if (IsCharFromCJK(theUChar))
    {
      return Font_UnicodeSubset_CJK;
    }
    else if (IsCharFromKorean(theUChar))
    {
      return Font_UnicodeSubset_Korean;
    }
    else if (IsCharFromArabic(theUChar))
    {
      return Font_UnicodeSubset_Arabic;
    }
    return Font_UnicodeSubset_Western;
  }

public:
  //! Create uninitialized instance.
  Standard_EXPORT Font_FTFont(
    const occ::handle<Font_FTLibrary>& theFTLib = occ::handle<Font_FTLibrary>());

  //! Destructor.
  Standard_EXPORT ~Font_FTFont() override;

  //! @return true if font is loaded
  inline bool IsValid() const { return myFTFace != nullptr; }

  //! @return image plane for currently rendered glyph
  inline const Image_PixMap& GlyphImage() const { return myGlyphImg; }

  //! Initialize the font from the given file path.
  //! @param theFontPath path to the font
  //! @param theParams   initialization parameters
  //! @param theFaceId   face id within the file (0 by default)
  //! @return true on success; false leaves the currently loaded face unchanged
  bool Init(const TCollection_AsciiString& theFontPath,
            const Params&                  theParams,
            const int                      theFaceId = 0)
  {
    return Init(occ::handle<NCollection_Buffer>(), theFontPath, theParams, theFaceId);
  }

  //! Initialize the font from the given file path or memory buffer.
  //! @param theData     memory to read from, should NOT be freed after initialization!
  //!                    when NULL, function will attempt to open theFileName file
  //! @param theFileName optional path to the font
  //! @param theParams   initialization parameters
  //! @param theFaceId   face id within the file (0 by default)
  //! @return true on success; false leaves the currently loaded face unchanged
  Standard_EXPORT bool Init(const occ::handle<NCollection_Buffer>& theData,
                            const TCollection_AsciiString&         theFileName,
                            const Params&                          theParams,
                            const int                              theFaceId = 0);

  //! Find (using Font_FontMgr) and initialize the font from the given name.
  //! @param theFontName    the font name
  //! @param theFontAspect  the font style
  //! @param theParams      initialization parameters
  //! @param theStrictLevel search strict level for using aliases and fallback
  //! @return true on success; false leaves the currently loaded face unchanged
  Standard_EXPORT bool FindAndInit(const TCollection_AsciiString& theFontName,
                                   Font_FontAspect                theFontAspect,
                                   const Params&                  theParams,
                                   Font_StrictLevel theStrictLevel = Font_StrictLevel_Any);

  //! Return flag to use fallback fonts in case if used font does not include symbols from specific
  //! Unicode subset; TRUE by default.
  //! @sa Font_FontMgr::ToUseUnicodeSubsetFallback()
  bool ToUseUnicodeSubsetFallback() const { return myToUseUnicodeSubsetFallback; }

  //! Set if fallback fonts should be used in case if used font does not include symbols from
  //! specific Unicode subset.
  //! Changing this policy invalidates previously resolved glyph identities.
  //! @param[in] theToFallback true to resolve missing characters through subset fallback faces
  Standard_EXPORT void SetUseUnicodeSubsetFallback(bool theToFallback);

  //! Return TRUE if this is single-stroke (one-line) font, FALSE by default.
  //! Such fonts define single-line glyphs instead of closed contours, so that they are rendered
  //! incorrectly by normal software.
  bool IsSingleStrokeFont() const { return myFontParams.IsSingleStrokeFont; }

  //! Set if this font should be rendered as single-stroke (one-line).
  //! @param[in] theIsSingleLine true to interpret contours as independent strokes
  Standard_EXPORT void SetSingleStrokeFont(bool theIsSingleLine);

  //! Return TRUE if italic style should be synthesized; FALSE by default.
  bool ToSynthesizeItalic() const { return myFontParams.ToSynthesizeItalic; }

  //! Release currently loaded font.
  Standard_EXPORT virtual void Release();

  //! Render specified glyph into internal buffer (bitmap).
  Standard_EXPORT bool RenderGlyph(const char32_t theChar);

  //! @param[in] theToIncludeFallback include initialized fallback faces
  //! @return maximal glyph width in pixels, or zero when the font is unavailable
  Standard_EXPORT unsigned int GlyphMaxSizeX(bool theToIncludeFallback = false) const;

  //! @param[in] theToIncludeFallback include initialized fallback faces
  //! @return maximal glyph height in pixels, or zero when the font is unavailable
  Standard_EXPORT unsigned int GlyphMaxSizeY(bool theToIncludeFallback = false) const;

  //! @return ascender in configured font units, or zero when the font is unavailable
  Standard_EXPORT float Ascender() const;

  //! @return descender in configured font units, or zero when the font is unavailable
  Standard_EXPORT float Descender() const;

  //! @return baseline-to-baseline spacing, or zero when the font is unavailable
  Standard_EXPORT float LineSpacing() const;

  //! Configured point size
  unsigned int PointSize() const { return myFontParams.PointSize; }

  //! Return glyph scaling along X-axis.
  float WidthScaling() const { return myWidthScaling; }

  //! Setup glyph scaling along X-axis.
  //! By default glyphs are not scaled (scaling factor = 1.0)
  //! @param[in] theScaleFactor horizontal outline and advance scale
  //! @return true when the positive scale is accepted; false when the input is invalid
  Standard_EXPORT bool SetWidthScaling(float theScaleFactor);

  //! Return TRUE if font contains specified symbol (excluding fallback list).
  //! @param[in] theUChar Unicode character
  //! @return true when the primary loaded face contains the character
  Standard_EXPORT bool HasSymbol(char32_t theUChar) const;

  //! Resolve a Unicode character to a primary or fallback face-local glyph.
  //! @param[in] theUChar Unicode character
  //! @return glyph identity; empty when the font is unavailable or the character is zero
  Standard_EXPORT std::optional<Font_GlyphOutline::Glyph> ResolveGlyph(char32_t theUChar);

  //! Return whether a resolved glyph belongs to the current font configuration.
  //! @param[in] theGlyph resolved glyph identity
  //! @return true when the identity can still be loaded by this font
  [[nodiscard]] Standard_EXPORT bool IsGlyphValid(const Font_GlyphOutline::Glyph& theGlyph) const;

  //! Extract an outline for a Unicode character.
  //! Fallback-face selection follows the same policy as bitmap glyph loading.
  //! Coordinates are returned in the selected face's design units.
  //! This method mutates the FreeType glyph slot and is not thread-safe.
  //! @param[in] theUChar Unicode character
  //! @return outline, or empty when loading or decomposition fails
  [[nodiscard]] Standard_EXPORT std::optional<Font_GlyphOutline> LoadGlyphOutline(
    char32_t theUChar);

  //! Extract an outline by resolved glyph identity.
  //! Coordinates are returned in the selected face's design units.
  //! This method mutates the FreeType glyph slot and is not thread-safe.
  //! @param[in] theGlyph face-local glyph identity returned by ResolveGlyph()
  //! @return outline, or empty when the glyph is stale or decomposition fails
  [[nodiscard]] Standard_EXPORT std::optional<Font_GlyphOutline> LoadGlyphOutline(
    const Font_GlyphOutline::Glyph& theGlyph);

  //! Compute horizontal advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  //! @param[in] theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceX(char32_t theUCharNext) const;

  //! Compute horizontal advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  //! @param[in] theUChar     the character to be loaded as current one
  //! @param[in] theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceX(char32_t theUChar, char32_t theUCharNext);

  //! Compute vertical advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  //! @param[in] theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceY(char32_t theUCharNext) const;

  //! Compute vertical advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  //! @param[in] theUChar     the character to be loaded as current one
  //! @param[in] theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceY(char32_t theUChar, char32_t theUCharNext);

  //! Return glyphs number in this font.
  //! @param[in] theToIncludeFallback include initialized fallback faces
  //! @return glyph count, or zero when the font is unavailable
  Standard_EXPORT int GlyphsNumber(bool theToIncludeFallback = false) const;

  //! Retrieve glyph bitmap rectangle, or a zero rectangle when no glyph is loaded.
  //! @param[out] theRect bitmap bounds
  Standard_EXPORT void GlyphRect(Font_Rect& theRect) const;

  //! Computes bounding box of the given text using plain-text formatter (Font_TextFormatter).
  //! Note that bounding box takes into account the text alignment options.
  //! Its corners are relative to the text alignment anchor point, their coordinates can be
  //! negative.
  Standard_EXPORT Font_Rect BoundingBox(const NCollection_String&               theString,
                                        const Graphic3d_HorizontalTextAlignment theAlignX,
                                        const Graphic3d_VerticalTextAlignment   theAlignY);

public:
  //! Initialize the font.
  //! @param theFontPath   path to the font
  //! @param thePointSize  the face size in points (1/72 inch)
  //! @param theResolution the resolution of the target device in dpi
  //! @return true on success
  Standard_DEPRECATED("Deprecated method, Font_FTFont::Params should be used for passing "
                      "parameters")
  bool Init(const NCollection_String& theFontPath,
            unsigned int              thePointSize,
            unsigned int              theResolution)
  {
    Params aParams;
    aParams.PointSize  = thePointSize;
    aParams.Resolution = theResolution;
    return Init(theFontPath.ToCString(), aParams, 0);
  }

  //! Initialize the font.
  //! @param theFontName   the font name
  //! @param theFontAspect the font style
  //! @param thePointSize  the face size in points (1/72 inch)
  //! @param theResolution the resolution of the target device in dpi
  //! @return true on success
  Standard_DEPRECATED("Deprecated method, Font_FTFont::Params should be used for passing "
                      "parameters")
  bool Init(const NCollection_String& theFontName,
            Font_FontAspect           theFontAspect,
            unsigned int              thePointSize,
            unsigned int              theResolution)
  {
    Params aParams;
    aParams.PointSize  = thePointSize;
    aParams.Resolution = theResolution;
    return FindAndInit(theFontName.ToCString(), theFontAspect, aParams);
  }

private:
  //! Load glyph without rendering it.
  bool loadGlyph(char32_t theUChar);

  //! Wrapper for FT_Get_Kerning - retrieve kerning values.
  bool getKerning(FT_Vector& theKern, char32_t theUCharCurr, char32_t theUCharNext) const;

  //! Initialize fallback font.
  bool findAndInitFallback(Font_UnicodeSubset theSubset);

  //! Face and face-local index selected for one Unicode character.
  class GlyphSelection
  {
  public:
    GlyphSelection(Font_FTFont* theProvider, const uint32_t theFace, const uint32_t theIndex)
        : Provider(theProvider),
          Face(theFace),
          Index(theIndex)
    {
    }

    Font_FTFont* Provider;
    uint32_t     Face;
    uint32_t     Index;
  };

  //! Coherent state of the FreeType slot currently exposed through bitmap/advance APIs.
  class LoadedGlyph
  {
  public:
    void Reset() noexcept
    {
      Face      = nullptr;
      Character = 0;
      HasBitmap = false;
    }

    FT_Face  Face      = nullptr;
    char32_t Character = 0;
    bool     HasBitmap = false;
  };

  [[nodiscard]] std::optional<GlyphSelection> selectGlyph(char32_t theUChar);
  [[nodiscard]] const Font_FTFont* fontForGlyph(const Font_GlyphOutline::Glyph& theGlyph) const;
  void                             invalidateGlyphState();

  occ::handle<Font_FTLibrary>     myFTLib;  //!< handle to the FT library object
  occ::handle<NCollection_Buffer> myBuffer; //!< memory buffer
  std::array<occ::handle<Font_FTFont>, Font_UnicodeSubset_NB>
                          myFallbackFaces; //!< fallback fonts by Unicode subset
  FT_Face                 myFTFace;        //!< FT face object
  TCollection_AsciiString myFontPath;      //!< font path
  Params                  myFontParams;    //!< font initialization parameters
  Font_FontAspect         myFontAspect;    //!< font initialization aspect
  float                   myWidthScaling;  //!< scale glyphs along X-axis
  int32_t                 myLoadFlags;     //!< default load flags
  std::shared_ptr<const Font_GlyphOutline::Glyph::Identity>
           myIdentity;     //!< lifetime-safe owner of resolved glyph identities
  uint64_t myRevision = 1; //!< font-local geometry revision

  Image_PixMap myGlyphImg;    //!< cached glyph plane
  LoadedGlyph  myLoadedGlyph; //!< currently loaded FreeType slot and its exposed state
  bool         myToUseUnicodeSubsetFallback; //!< use subset fallback faces for missing characters
};

#endif // Font_FTFont_HeaderFile
