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

#include <Font_FTFont.hxx>

IMPLEMENT_STANDARD_HANDLE (Font_FTFont, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Font_FTFont, Standard_Transient)

// =======================================================================
// function : Font_FTFont
// purpose  :
// =======================================================================
Font_FTFont::Font_FTFont (const Handle(Font_FTLibrary)& theFTLib)
: myFTLib (theFTLib),
  myFTFace (NULL),
  myPointSize (0),
  myUChar (0)
{
  if (myFTLib.IsNull())
  {
    myFTLib = new Font_FTLibrary();
  }
}

// =======================================================================
// function : Font_FTFont
// purpose  :
// =======================================================================
Font_FTFont::~Font_FTFont()
{
  Release();
}

// =======================================================================
// function : Font_FTFont
// purpose  :
// =======================================================================
void Font_FTFont::Release()
{
  myGlyphImg.Clear();
  myFontPath.Clear();
  myUChar = 0;
  if (myFTFace != NULL)
  {
    FT_Done_Face (myFTFace);
    myFTFace = NULL;
  }
}

// =======================================================================
// function : Font_FTFont
// purpose  :
// =======================================================================
bool Font_FTFont::Init (const NCollection_String& theFontPath,
                        const unsigned int        thePointSize,
                        const unsigned int        theResolution)
{
  Release();
  myFontPath  = theFontPath;
  myPointSize = thePointSize;
  if (!myFTLib->IsValid())
  {
    std::cerr << "FreeType library is unavailable!\n";
    Release();
    return false;
  }

  if (FT_New_Face (myFTLib->Instance(), myFontPath.ToCString(), 0, &myFTFace) != 0)
  {
    //std::cerr << "Font '" << myFontPath << "' fail to load!\n";
    Release();
    return false;
  }
  else if (FT_Select_Charmap (myFTFace, ft_encoding_unicode) != 0)
  {
    //std::cerr << "Font '" << myFontPath << "' doesn't contains Unicode charmap!\n";
    Release();
    return false;
  }
  else if (FT_Set_Char_Size (myFTFace, 0L, toFTPoints (thePointSize), theResolution, theResolution) != 0)
  {
    //std::cerr << "Font '" << myFontPath << "' doesn't contains Unicode charmap!\n";
    Release();
    return false;
  }
  return true;
}

// =======================================================================
// function : loadGlyph
// purpose  :
// =======================================================================
bool Font_FTFont::loadGlyph (const Standard_Utf32Char theUChar)
{
  if (myUChar == theUChar)
  {
    return true;
  }

  myGlyphImg.Clear();
  myUChar = 0;
  if (theUChar == 0
   || FT_Load_Char (myFTFace, theUChar, FT_LOAD_TARGET_NORMAL) != 0
   || myFTFace->glyph == NULL)
  {
    return false;
  }

  myUChar = theUChar;
  return true;
}

// =======================================================================
// function : RenderGlyph
// purpose  :
// =======================================================================
bool Font_FTFont::RenderGlyph (const Standard_Utf32Char theUChar)
{
  myGlyphImg.Clear();
  myUChar = 0;
  if (theUChar == 0
   || FT_Load_Char (myFTFace, theUChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_TARGET_NORMAL) != 0
   || myFTFace->glyph == NULL
   || myFTFace->glyph->format != FT_GLYPH_FORMAT_BITMAP)
  {
    return false;
  }

  FT_Bitmap aBitmap = myFTFace->glyph->bitmap;
  if (aBitmap.pixel_mode != FT_PIXEL_MODE_GRAY
   || aBitmap.buffer == NULL || aBitmap.width <= 0 || aBitmap.rows <= 0)
  {
    return false;
  }
  if (!myGlyphImg.InitWrapper (Image_PixMap::ImgGray, aBitmap.buffer,
                               aBitmap.width, aBitmap.rows, Abs (aBitmap.pitch)))
  {
    return false;
  }
  myGlyphImg.SetTopDown (aBitmap.pitch > 0);
  myUChar = theUChar;
  return true;
}

// =======================================================================
// function : GlyphMaxSizeX
// purpose  :
// =======================================================================
unsigned int Font_FTFont::GlyphMaxSizeX() const
{
  float aWidth = (FT_IS_SCALABLE(myFTFace) != 0)
               ? float(myFTFace->bbox.xMax - myFTFace->bbox.xMin) * (float(myFTFace->size->metrics.x_ppem) / float(myFTFace->units_per_EM))
               : fromFTPoints<float> (myFTFace->size->metrics.max_advance);
  return (unsigned int)(aWidth + 0.5f);
}

// =======================================================================
// function : GlyphMaxSizeY
// purpose  :
// =======================================================================
unsigned int Font_FTFont::GlyphMaxSizeY() const
{
  float aHeight = (FT_IS_SCALABLE(myFTFace) != 0)
                ? float(myFTFace->bbox.yMax - myFTFace->bbox.yMin) * (float(myFTFace->size->metrics.y_ppem) / float(myFTFace->units_per_EM))
                : fromFTPoints<float> (myFTFace->size->metrics.height);
  return (unsigned int)(aHeight + 0.5f);
}

// =======================================================================
// function : AdvanceX
// purpose  :
// =======================================================================
float Font_FTFont::AdvanceX (const Standard_Utf32Char theUChar,
                             const Standard_Utf32Char theUCharNext)
{
  loadGlyph (theUChar);
  return AdvanceX (theUCharNext);
}

// =======================================================================
// function : AdvanceY
// purpose  :
// =======================================================================
float Font_FTFont::AdvanceY (const Standard_Utf32Char theUChar,
                             const Standard_Utf32Char theUCharNext)
{
  loadGlyph (theUChar);
  return AdvanceY (theUCharNext);
}

// =======================================================================
// function : AdvanceX
// purpose  :
// =======================================================================
float Font_FTFont::AdvanceX (const Standard_Utf32Char theUCharNext)
{
  if (myUChar == 0)
  {
    return 0.0f;
  }

  if (FT_HAS_KERNING (myFTFace) == 0 || theUCharNext == 0
   || FT_Get_Kerning (myFTFace, myUChar, theUCharNext, FT_KERNING_UNFITTED, &myKernAdvance) != 0)
  {
    return fromFTPoints<float> (myFTFace->glyph->advance.x);
  }
  return fromFTPoints<float> (myKernAdvance.x + myFTFace->glyph->advance.x);
}

// =======================================================================
// function : AdvanceY
// purpose  :
// =======================================================================
float Font_FTFont::AdvanceY (const Standard_Utf32Char theUCharNext)
{
  if (myUChar == 0)
  {
    return 0.0f;
  }

  if (FT_HAS_KERNING (myFTFace) == 0 || theUCharNext == 0
   || FT_Get_Kerning (myFTFace, myUChar, theUCharNext, FT_KERNING_UNFITTED, &myKernAdvance) != 0)
  {
    return fromFTPoints<float> (myFTFace->glyph->advance.y);
  }
  return fromFTPoints<float> (myKernAdvance.y + myFTFace->glyph->advance.y);
}
