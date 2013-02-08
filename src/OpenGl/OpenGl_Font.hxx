// Created on: 2013-01-29
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

#ifndef _OpenGl_Font_H__
#define _OpenGl_Font_H__

#include <OpenGl_Texture.hxx>
#include <OpenGl_Vec.hxx>

#include <Font_FTFont.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>

//! Texture font.
class OpenGl_Font : public OpenGl_Resource
{

public:

  //! Simple structure stores tile rectangle.
  struct Tile
  {
    Font_FTFont::Rect uv;      //!< UV coordinates in texture
    Font_FTFont::Rect px;      //!< pixel displacement coordinates
    GLuint            texture; //!< GL texture ID
  };

  struct RectI
  {
    Standard_Integer Left;
    Standard_Integer Right;
    Standard_Integer Top;
    Standard_Integer Bottom;
  };

public:

  //! Main constructor.
  Standard_EXPORT OpenGl_Font (const Handle(Font_FTFont)&     theFont,
                               const TCollection_AsciiString& theKey = "");

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_Font();

  //! Destroy object - will release GPU memory if any
  Standard_EXPORT virtual void Release (const OpenGl_Context* theCtx);

  //! @return key of shared resource
  inline const TCollection_AsciiString& ResourceKey() const
  {
    return myKey;
  }

  //! @return FreeType font instance specified on construction.
  inline const Handle(Font_FTFont)& FTFont() const
  {
    return myFont;
  }

  //! @return true if font was loaded successfully.
  inline bool IsValid() const
  {
    return !myTextures.IsEmpty() && myTextures.First()->IsValid();
  }

  //! Notice that this method doesn't return initialization success state.
  //! Use IsValid() instead.
  //! @return true if initialization was already called.
  inline bool WasInitialized() const
  {
    return !myTextures.IsEmpty();
  }

  //! Initialize GL resources.
  //! FreeType font instance should be already initialized!
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theCtx);

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  inline float AdvanceX (const Standard_Utf32Char theUChar,
                         const Standard_Utf32Char theUCharNext)
  {
    return myFont->AdvanceX (theUChar, theUCharNext);
  }

  //! @return vertical distance from the horizontal baseline to the highest character coordinate
  inline float Ascender() const
  {
    return myAscender;
  }

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate
  inline float Descender() const
  {
    return myDescender;
  }

  //! @return default line spacing (the baseline-to-baseline distance)
  inline float LineSpacing() const
  {
    return myLineSpacing;
  }

  //! Compute glyph rectangle at specified pen position (on baseline)
  //! and render it to texture if not already.
  //! @param theCtx       active context
  //! @param theUChar     unicode symbol to render
  //! @param theUCharNext next symbol to compute advance with kerning when available
  //! @param theGlyph     computed glyph position rectangle, texture ID and UV coordinates
  //! @param thePen       pen position on baseline to place new glyph
  Standard_EXPORT void RenderGlyph (const Handle(OpenGl_Context)& theCtx,
                                    const Standard_Utf32Char      theUChar,
                                    const Standard_Utf32Char      theUCharNext,
                                    Tile&                         theGlyph,
                                    OpenGl_Vec2&                  thePen);

protected:

  //! Render new glyph to the texture.
  bool renderGlyph (const Handle(OpenGl_Context)& theCtx,
                    const Standard_Utf32Char      theChar);

  //! Allocate new texture.
  bool createTexture (const Handle(OpenGl_Context)& theCtx);

protected:

  TCollection_AsciiString myKey;           //!< key of shared resource
  Handle(Font_FTFont)     myFont;          //!< FreeType font instance
  Standard_ShortReal      myAscender;      //!< ascender     provided my FT font
  Standard_ShortReal      myDescender;     //!< descender    provided my FT font
  Standard_ShortReal      myLineSpacing;   //!< line spacing provided my FT font
  Standard_Integer        myTileSizeX;     //!< tile width
  Standard_Integer        myTileSizeY;     //!< tile height
  Standard_Integer        myLastTileId;    //!< id of last tile
  RectI                   myLastTilePx;
  Standard_Integer        myTextureFormat; //!< texture format

  NCollection_Vector<Handle(OpenGl_Texture)> myTextures; //!< array of textures
  NCollection_Vector<Tile>                   myTiles;    //!< array of loaded tiles

  NCollection_DataMap<Standard_Utf32Char, Standard_Integer> myGlyphMap;

public:

  DEFINE_STANDARD_RTTI(OpenGl_Font) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Font, OpenGl_Resource)

#endif // _OpenGl_Font_H__
