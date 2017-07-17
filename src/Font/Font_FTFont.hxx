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

#ifndef _Font_FTFont_H__
#define _Font_FTFont_H__

#include <Font_FontAspect.hxx>
#include <Font_Rect.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Image_PixMap.hxx>
#include <NCollection_String.hxx>

// forward declarations to avoid including of FreeType headers
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_Vector_   FT_Vector;
class Font_FTLibrary;

//! Wrapper over FreeType font.
//! Notice that this class uses internal buffers for loaded glyphs
//! and it is absolutely UNSAFE to load/read glyph from concurrent threads!
class Font_FTFont : public Standard_Transient
{
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

  //! Initialize the font.
  //! @param theFontPath   path to the font
  //! @param thePointSize  the face size in points (1/72 inch)
  //! @param theResolution the resolution of the target device in dpi
  //! @return true on success
  Standard_EXPORT bool Init (const NCollection_String& theFontPath,
                             const unsigned int        thePointSize,
                             const unsigned int        theResolution);

  //! Initialize the font.
  //! @param theFontName   the font name
  //! @param theFontAspect the font style
  //! @param thePointSize  the face size in points (1/72 inch)
  //! @param theResolution the resolution of the target device in dpi
  //! @return true on success
  Standard_EXPORT bool Init (const NCollection_String& theFontName,
                             const Font_FontAspect     theFontAspect,
                             const unsigned int        thePointSize,
                             const unsigned int        theResolution);

  //! Release currently loaded font.
  Standard_EXPORT virtual void Release();

  //! Render specified glyph into internal buffer (bitmap).
  Standard_EXPORT bool RenderGlyph (const Standard_Utf32Char theChar);

  //! @return maximal glyph width in pixels (rendered to bitmap).
  Standard_EXPORT unsigned int GlyphMaxSizeX() const;

  //! @return maximal glyph height in pixels (rendered to bitmap).
  Standard_EXPORT unsigned int GlyphMaxSizeY() const;

  //! @return vertical distance from the horizontal baseline to the highest character coordinate.
  Standard_EXPORT float Ascender() const;

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate.
  Standard_EXPORT float Descender() const;

  //! @return default line spacing (the baseline-to-baseline distance).
  Standard_EXPORT float LineSpacing() const;

  //! Configured point size
  unsigned int PointSize() const
  {
    return myPointSize;
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  Standard_EXPORT float AdvanceX (const Standard_Utf32Char theUCharNext);

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  Standard_EXPORT float AdvanceX (const Standard_Utf32Char theUChar,
                                  const Standard_Utf32Char theUCharNext);

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  Standard_EXPORT float AdvanceY (const Standard_Utf32Char theUCharNext);

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  Standard_EXPORT float AdvanceY (const Standard_Utf32Char theUChar,
                                  const Standard_Utf32Char theUCharNext);

  //! @return glyphs number in this font.
  Standard_EXPORT Standard_Integer GlyphsNumber() const;

  //! Retrieve glyph bitmap rectangle
  Standard_EXPORT void GlyphRect (Font_Rect& theRect) const;

  //! Computes bounding box of the given text using plain-text formatter (Font_TextFormatter).
  //! Note that bounding box takes into account the text alignment options.
  //! Its corners are relative to the text alignment anchor point, their coordinates can be negative.
  Standard_EXPORT Font_Rect BoundingBox (const NCollection_String&               theString,
                                         const Graphic3d_HorizontalTextAlignment theAlignX,
                                         const Graphic3d_VerticalTextAlignment   theAlignY);

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

protected:

  Handle(Font_FTLibrary) myFTLib;       //!< handle to the FT library object
  FT_Face                myFTFace;      //!< FT face object
  NCollection_String     myFontPath;    //!< font path
  unsigned int           myPointSize;   //!< point size set by FT_Set_Char_Size
  int32_t                myLoadFlags;   //!< default load flags

  Image_PixMap           myGlyphImg;    //!< cached glyph plane
  FT_Vector*             myKernAdvance; //!< buffer variable
  Standard_Utf32Char     myUChar;       //!< currently loaded unicode character

public:

  DEFINE_STANDARD_RTTIEXT(Font_FTFont,Standard_Transient) // Type definition

};

DEFINE_STANDARD_HANDLE(Font_FTFont, Standard_Transient)

#endif // _Font_FTFont_H__
