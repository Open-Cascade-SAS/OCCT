// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Font_FTFont_H__
#define _Font_FTFont_H__

#include <NCollection_Vec2.hxx>
#include <NCollection_String.hxx>
#include <Font_FTLibrary.hxx>
#include <Image_PixMap.hxx>

//! Wrapper over FreeType font.
//! Notice that this class uses internal buffers for loaded glyphs
//! and it is absolutely UNSAFE to load/read glyph from concurrent threads!
class Font_FTFont : public Standard_Transient
{

public:

  //! Auxiliary structure - rectangle definition
  struct Rect
  {
    float Left;
    float Right;
    float Top;
    float Bottom;

    NCollection_Vec2<float>& TopLeft (NCollection_Vec2<float>& theVec) const
    {
      theVec.x() = Left;
      theVec.y() = Top;
      return theVec;
    }

    NCollection_Vec2<float>& TopRight (NCollection_Vec2<float>& theVec) const
    {
      theVec.x() = Right;
      theVec.y() = Top;
      return theVec;
    }

    NCollection_Vec2<float>& BottomLeft (NCollection_Vec2<float>& theVec) const
    {
      theVec.x() = Left;
      theVec.y() = Bottom;
      return theVec;
    }

    NCollection_Vec2<float>& BottomRight (NCollection_Vec2<float>& theVec) const
    {
      theVec.x() = Right;
      theVec.y() = Bottom;
      return theVec;
    }

  };

public:

  //! Create uninitialized instance.
  Standard_EXPORT Font_FTFont (const Handle(Font_FTLibrary)& theFTLib = NULL);

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
                             const unsigned int        theResolution = 72);

  //! Release currently loaded font.
  Standard_EXPORT void Release();

  //! Render specified glyph into internal buffer (bitmap).
  Standard_EXPORT bool RenderGlyph (const Standard_Utf32Char theChar);

  //! @return maximal glyph width in pixels (rendered to bitmap).
  Standard_EXPORT unsigned int GlyphMaxSizeX() const;

  //! @return maximal glyph height in pixels (rendered to bitmap).
  Standard_EXPORT unsigned int GlyphMaxSizeY() const;

  //! @return vertical distance from the horizontal baseline to the highest character coordinate.
  inline float Ascender() const
  {
    return float(myFTFace->ascender) * (float(myFTFace->size->metrics.y_ppem) / float(myFTFace->units_per_EM));
  }

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate.
  inline float Descender() const
  {
    return float(myFTFace->descender) * (float(myFTFace->size->metrics.y_ppem) / float(myFTFace->units_per_EM));
  }

  //! @return default line spacing (the baseline-to-baseline distance).
  inline float LineSpacing() const
  {
    return float(myFTFace->height) * (float(myFTFace->size->metrics.y_ppem) / float(myFTFace->units_per_EM));
  }

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
  inline Standard_Integer GlyphsNumber() const
  {
    return myFTFace->num_glyphs;
  }

  //! Retrieve glyph bitmap rectangle
  inline void GlyphRect (Font_FTFont::Rect& theRect) const
  {
    FT_Bitmap aBitmap = myFTFace->glyph->bitmap;
    theRect.Left   = float(myFTFace->glyph->bitmap_left);
    theRect.Top    = float(myFTFace->glyph->bitmap_top);
    theRect.Right  = float(myFTFace->glyph->bitmap_left + aBitmap.width);
    theRect.Bottom = float(myFTFace->glyph->bitmap_top  - aBitmap.rows);
  }

protected:

  //! Convert value to 26.6 fixed-point format for FT library API.
  template <typename theInput_t>
  inline FT_F26Dot6 toFTPoints (const theInput_t thePointSize) const
  {
    return (FT_F26Dot6)thePointSize * 64; 
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

  Image_PixMap           myGlyphImg;    //!< cached glyph plane
  FT_Vector              myKernAdvance; //!< buffer variable
  Standard_Utf32Char     myUChar;       //!< currently loaded unicode character

public:

  DEFINE_STANDARD_RTTI(Font_FTFont) // Type definition

};

DEFINE_STANDARD_HANDLE(Font_FTFont, Standard_Transient)

#endif // _Font_FTFont_H__
