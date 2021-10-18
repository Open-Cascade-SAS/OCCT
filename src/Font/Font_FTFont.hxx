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
#include <Font_Hinting.hxx>
#include <Font_Rect.hxx>
#include <Font_StrictLevel.hxx>
#include <Font_UnicodeSubset.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Image_PixMap.hxx>
#include <NCollection_String.hxx>
#include <TCollection_AsciiString.hxx>

// forward declarations to avoid including of FreeType headers
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_Vector_   FT_Vector;
typedef struct FT_Outline_ FT_Outline;
class Font_FTLibrary;

//! Font initialization parameters.
struct Font_FTFontParams
{
  unsigned int PointSize;                  //!< face size in points (1/72 inch)
  unsigned int Resolution;                 //!< resolution of the target device in dpi for FT_Set_Char_Size()
  Font_Hinting FontHinting;                //!< request hinting (exclude FT_LOAD_NO_HINTING flag), Font_Hinting_Off by default;
                                           //!  hinting improves readability of thin text on low-resolution screen,
                                           //!  but adds distortions to original font depending on font family and font library version
  bool         ToSynthesizeItalic;         //!< generate italic style (e.g. for font family having no italic style); FALSE by default
  bool         IsSingleStrokeFont;         //!< single-stroke (one-line) font, FALSE by default

  //! Empty constructor.
  Font_FTFontParams()
  : PointSize (0), Resolution (72u),
    FontHinting (Font_Hinting_Off),
    ToSynthesizeItalic (false),
    IsSingleStrokeFont (false)  {}

  //! Constructor.
  Font_FTFontParams (unsigned int thePointSize,
                     unsigned int theResolution)
  : PointSize (thePointSize), Resolution (theResolution),
    FontHinting (Font_Hinting_Off),
    ToSynthesizeItalic (false),
    IsSingleStrokeFont (false) {}
};

DEFINE_STANDARD_HANDLE(Font_FTFont, Standard_Transient)

//! Wrapper over FreeType font.
//! Notice that this class uses internal buffers for loaded glyphs
//! and it is absolutely UNSAFE to load/read glyph from concurrent threads!
class Font_FTFont : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Font_FTFont, Standard_Transient)
public:

  //! Find the font Initialize the font.
  //! @param theFontName    the font name
  //! @param theFontAspect  the font style
  //! @param theParams      initialization parameters
  //! @param theStrictLevel search strict level for using aliases and fallback
  //! @return true on success
  Standard_EXPORT static Handle(Font_FTFont) FindAndCreate (const TCollection_AsciiString& theFontName,
                                                            const Font_FontAspect     theFontAspect,
                                                            const Font_FTFontParams&  theParams,
                                                            const Font_StrictLevel    theStrictLevel = Font_StrictLevel_Any);

  //! Return TRUE if specified character is within subset of modern CJK characters.
  static bool IsCharFromCJK (Standard_Utf32Char theUChar)
  {
    return (theUChar >= 0x03400 && theUChar <= 0x04DFF)
        || (theUChar >= 0x04E00 && theUChar <= 0x09FFF)
        || (theUChar >= 0x0F900 && theUChar <= 0x0FAFF)
        || (theUChar >= 0x20000 && theUChar <= 0x2A6DF)
        || (theUChar >= 0x2F800 && theUChar <= 0x2FA1F)
        // Hiragana and Katakana (Japanese) are NOT part of CJK, but CJK fonts usually include these symbols
        || IsCharFromHiragana (theUChar)
        || IsCharFromKatakana (theUChar);
  }

  //! Return TRUE if specified character is within subset of Hiragana (Japanese).
  static bool IsCharFromHiragana (Standard_Utf32Char theUChar)
  {
    return (theUChar >= 0x03040 && theUChar <= 0x0309F);
  }

  //! Return TRUE if specified character is within subset of Katakana (Japanese).
  static bool IsCharFromKatakana (Standard_Utf32Char theUChar)
  {
    return (theUChar >= 0x030A0 && theUChar <= 0x030FF);
  }

  //! Return TRUE if specified character is within subset of modern Korean characters (Hangul).
  static bool IsCharFromKorean (Standard_Utf32Char theUChar)
  {
    return (theUChar >= 0x01100 && theUChar <= 0x011FF)
        || (theUChar >= 0x03130 && theUChar <= 0x0318F)
        || (theUChar >= 0x0AC00 && theUChar <= 0x0D7A3);
  }

  //! Return TRUE if specified character is within subset of Arabic characters.
  static bool IsCharFromArabic (Standard_Utf32Char theUChar)
  {
    return (theUChar >= 0x00600 && theUChar <= 0x006FF);
  }

  //! Return TRUE if specified character should be displayed in Right-to-Left order.
  static bool IsCharRightToLeft (Standard_Utf32Char theUChar)
  {
    return IsCharFromArabic(theUChar);
  }

  //! Determine Unicode subset for specified character
  static Font_UnicodeSubset CharSubset (Standard_Utf32Char theUChar)
  {
    if (IsCharFromCJK (theUChar))
    {
      return Font_UnicodeSubset_CJK;
    }
    else if (IsCharFromKorean (theUChar))
    {
      return Font_UnicodeSubset_Korean;
    }
    else if (IsCharFromArabic (theUChar))
    {
      return Font_UnicodeSubset_Arabic;
    }
    return Font_UnicodeSubset_Western;
  }

public:

  //! Create uninitialized instance.
  Standard_EXPORT Font_FTFont (const Handle(Font_FTLibrary)& theFTLib = Handle(Font_FTLibrary)());

  //! Destructor.
  Standard_EXPORT virtual ~Font_FTFont();

  //! @return true if font is loaded
  inline bool IsValid() const
  {
    return myFTFace != NULL;
  }

  //! @return image plane for currently rendered glyph
  inline const Image_PixMap& GlyphImage() const
  {
    return myGlyphImg;
  }

  //! Initialize the font from the given file path.
  //! @param theFontPath path to the font
  //! @param theParams   initialization parameters
  //! @param theFaceId   face id within the file (0 by default)
  //! @return true on success
  bool Init (const TCollection_AsciiString& theFontPath,
             const Font_FTFontParams& theParams,
             const Standard_Integer theFaceId = 0)
  {
    return Init (Handle(NCollection_Buffer)(), theFontPath, theParams, theFaceId);
  }

  //! Initialize the font from the given file path or memory buffer.
  //! @param theData     memory to read from, should NOT be freed after initialization!
  //!                    when NULL, function will attempt to open theFileName file
  //! @param theFileName optional path to the font
  //! @param theParams   initialization parameters
  //! @param theFaceId   face id within the file (0 by default)
  //! @return true on success
  Standard_EXPORT bool Init (const Handle(NCollection_Buffer)& theData,
                             const TCollection_AsciiString& theFileName,
                             const Font_FTFontParams& theParams,
                             const Standard_Integer theFaceId = 0);

  //! Find (using Font_FontMgr) and initialize the font from the given name.
  //! @param theFontName    the font name
  //! @param theFontAspect  the font style
  //! @param theParams      initialization parameters
  //! @param theStrictLevel search strict level for using aliases and fallback
  //! @return true on success
  Standard_EXPORT bool FindAndInit (const TCollection_AsciiString& theFontName,
                                    Font_FontAspect theFontAspect,
                                    const Font_FTFontParams& theParams,
                                    Font_StrictLevel theStrictLevel = Font_StrictLevel_Any);

  //! Return flag to use fallback fonts in case if used font does not include symbols from specific Unicode subset; TRUE by default.
  //! @sa Font_FontMgr::ToUseUnicodeSubsetFallback()
  Standard_Boolean ToUseUnicodeSubsetFallback() const { return myToUseUnicodeSubsetFallback; }

  //! Set if fallback fonts should be used in case if used font does not include symbols from specific Unicode subset.
  void SetUseUnicodeSubsetFallback (Standard_Boolean theToFallback) { myToUseUnicodeSubsetFallback = theToFallback; }

  //! Return TRUE if this is single-stroke (one-line) font, FALSE by default.
  //! Such fonts define single-line glyphs instead of closed contours, so that they are rendered incorrectly by normal software.
  bool IsSingleStrokeFont() const { return myFontParams.IsSingleStrokeFont; }

  //! Set if this font should be rendered as single-stroke (one-line).
  void SetSingleStrokeFont (bool theIsSingleLine) { myFontParams.IsSingleStrokeFont = theIsSingleLine; }

  //! Return TRUE if italic style should be synthesized; FALSE by default.
  bool ToSynthesizeItalic() const { return myFontParams.ToSynthesizeItalic; }

  //! Release currently loaded font.
  Standard_EXPORT virtual void Release();

  //! Render specified glyph into internal buffer (bitmap).
  Standard_EXPORT bool RenderGlyph (const Standard_Utf32Char theChar);

  //! @return maximal glyph width in pixels (rendered to bitmap).
  Standard_EXPORT unsigned int GlyphMaxSizeX (bool theToIncludeFallback = false) const;

  //! @return maximal glyph height in pixels (rendered to bitmap).
  Standard_EXPORT unsigned int GlyphMaxSizeY (bool theToIncludeFallback = false) const;

  //! @return vertical distance from the horizontal baseline to the highest character coordinate.
  Standard_EXPORT float Ascender() const;

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate.
  Standard_EXPORT float Descender() const;

  //! @return default line spacing (the baseline-to-baseline distance).
  Standard_EXPORT float LineSpacing() const;

  //! Configured point size
  unsigned int PointSize() const
  {
    return myFontParams.PointSize;
  }

  //! Return glyph scaling along X-axis.
  float WidthScaling() const
  {
    return myWidthScaling;
  }

  //! Setup glyph scaling along X-axis.
  //! By default glyphs are not scaled (scaling factor = 1.0)
  void SetWidthScaling (const float theScaleFactor)
  {
    myWidthScaling = theScaleFactor;
  }

  //! Return TRUE if font contains specified symbol (excluding fallback list).
  Standard_EXPORT bool HasSymbol (Standard_Utf32Char theUChar) const;

  //! Compute horizontal advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  //! @param theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceX (Standard_Utf32Char theUCharNext) const;

  //! Compute horizontal advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  //! @param theUChar     the character to be loaded as current one
  //! @param theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceX (Standard_Utf32Char theUChar,
                                  Standard_Utf32Char theUCharNext);

  //! Compute vertical advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  //! @param theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceY (Standard_Utf32Char theUCharNext) const;

  //! Compute vertical advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  //! @param theUChar     the character to be loaded as current one
  //! @param theUCharNext the next character to compute advance from current one
  Standard_EXPORT float AdvanceY (Standard_Utf32Char theUChar,
                                  Standard_Utf32Char theUCharNext);

  //! Return glyphs number in this font.
  //! @param theToIncludeFallback if TRUE then the number will include fallback list
  Standard_EXPORT Standard_Integer GlyphsNumber (bool theToIncludeFallback = false) const;

  //! Retrieve glyph bitmap rectangle
  Standard_EXPORT void GlyphRect (Font_Rect& theRect) const;

  //! Computes bounding box of the given text using plain-text formatter (Font_TextFormatter).
  //! Note that bounding box takes into account the text alignment options.
  //! Its corners are relative to the text alignment anchor point, their coordinates can be negative.
  Standard_EXPORT Font_Rect BoundingBox (const NCollection_String&               theString,
                                         const Graphic3d_HorizontalTextAlignment theAlignX,
                                         const Graphic3d_VerticalTextAlignment   theAlignY);

public:

  //! Computes outline contour for the symbol.
  //! @param theUChar    [in] the character to be loaded as current one
  //! @param theOutline  [out] outline contour
  //! @return true on success
  Standard_EXPORT const FT_Outline* renderGlyphOutline(const Standard_Utf32Char theChar);

public:

  //! Initialize the font.
  //! @param theFontPath   path to the font
  //! @param thePointSize  the face size in points (1/72 inch)
  //! @param theResolution the resolution of the target device in dpi
  //! @return true on success
  Standard_DEPRECATED ("Deprecated method, Font_FTFontParams should be used for passing parameters")
  bool Init (const NCollection_String& theFontPath,
             unsigned int thePointSize,
             unsigned int theResolution)
  {
    Font_FTFontParams aParams;
    aParams.PointSize  = thePointSize;
    aParams.Resolution = theResolution;
    return Init (theFontPath.ToCString(), aParams, 0);
  }

  //! Initialize the font.
  //! @param theFontName   the font name
  //! @param theFontAspect the font style
  //! @param thePointSize  the face size in points (1/72 inch)
  //! @param theResolution the resolution of the target device in dpi
  //! @return true on success
  Standard_DEPRECATED ("Deprecated method, Font_FTFontParams should be used for passing parameters")
  bool Init (const NCollection_String& theFontName,
             Font_FontAspect theFontAspect,
             unsigned int thePointSize,
             unsigned int theResolution)
  {
    Font_FTFontParams aParams;
    aParams.PointSize  = thePointSize;
    aParams.Resolution = theResolution;
    return FindAndInit (theFontName.ToCString(), theFontAspect, aParams);
  }

protected:

  //! Convert value to 26.6 fixed-point format for FT library API.
  template <typename theInput_t>
  int32_t toFTPoints (const theInput_t thePointSize) const
  {
    return (int32_t)thePointSize * 64;
  }

  //! Convert value from 26.6 fixed-point format for FT library API.
  template <typename theReturn_t, typename theFTUnits_t>
  inline theReturn_t fromFTPoints (const theFTUnits_t theFTUnits) const
  {
    return (theReturn_t)theFTUnits / 64.0f;
  }

protected:

  //! Load glyph without rendering it.
  Standard_EXPORT bool loadGlyph (const Standard_Utf32Char theUChar);

  //! Wrapper for FT_Get_Kerning - retrieve kerning values.
  Standard_EXPORT bool getKerning (FT_Vector& theKern,
                                   Standard_Utf32Char theUCharCurr,
                                   Standard_Utf32Char theUCharNext) const;

  //! Initialize fallback font.
  Standard_EXPORT bool findAndInitFallback (Font_UnicodeSubset theSubset);

  //! Enable/disable load flag.
  void setLoadFlag (int32_t theFlag, bool theToEnable)
  {
    if (theToEnable)
    {
      myLoadFlags |= theFlag;
    }
    else
    {
      myLoadFlags &= ~theFlag;
    }
  }

protected:

  Handle(Font_FTLibrary)     myFTLib;        //!< handle to the FT library object
  Handle(NCollection_Buffer) myBuffer;       //!< memory buffer
  Handle(Font_FTFont)        myFallbackFaces[Font_UnicodeSubset_NB]; //!< fallback fonts
  FT_Face                    myFTFace;       //!< FT face object
  FT_Face                    myActiveFTFace; //!< active FT face object (the main of fallback)
  TCollection_AsciiString    myFontPath;     //!< font path
  Font_FTFontParams          myFontParams;   //!< font initialization parameters
  Font_FontAspect            myFontAspect;   //!< font initialization aspect
  float                      myWidthScaling; //!< scale glyphs along X-axis
  int32_t                    myLoadFlags;    //!< default load flags

  Image_PixMap               myGlyphImg;     //!< cached glyph plane
  Standard_Utf32Char         myUChar;        //!< currently loaded unicode character
  Standard_Boolean           myToUseUnicodeSubsetFallback; //!< use default fallback fonts for extended Unicode sub-sets (Korean, CJK, etc.)

};

#endif // _Font_FTFont_H__
