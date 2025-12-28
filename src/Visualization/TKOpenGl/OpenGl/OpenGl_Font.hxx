// Created on: 2013-01-29
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

#ifndef OpenGl_Font_HeaderFile
#define OpenGl_Font_HeaderFile

#include <OpenGl_Texture.hxx>
#include <OpenGl_Vec.hxx>

#include <Font_Rect.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>

class Font_FTFont;

//! Texture font.
class OpenGl_Font : public OpenGl_Resource
{

public:
  //! Simple structure stores tile rectangle.
  struct Tile
  {
    Font_Rect uv;      //!< UV coordinates in texture
    Font_Rect px;      //!< pixel displacement coordinates
    GLuint    texture; //!< GL texture ID
  };

  struct RectI
  {
    int Left;
    int Right;
    int Top;
    int Bottom;
  };

public:
  //! Main constructor.
  Standard_EXPORT OpenGl_Font(const occ::handle<Font_FTFont>& theFont,
                              const TCollection_AsciiString&  theKey = "");

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_Font();

  //! Destroy object - will release GPU memory if any
  Standard_EXPORT virtual void Release(OpenGl_Context* theCtx) override;

  //! Returns estimated GPU memory usage.
  Standard_EXPORT virtual size_t EstimatedDataSize() const override;

  //! @return key of shared resource
  inline const TCollection_AsciiString& ResourceKey() const { return myKey; }

  //! @return FreeType font instance specified on construction.
  inline const occ::handle<Font_FTFont>& FTFont() const { return myFont; }

  //! @return true if font was loaded successfully.
  inline bool IsValid() const { return !myTextures.IsEmpty() && myTextures.First()->IsValid(); }

  //! Notice that this method doesn't return initialization success state.
  //! Use IsValid() instead.
  //! @return true if initialization was already called.
  inline bool WasInitialized() const { return !myTextures.IsEmpty(); }

  //! Initialize GL resources.
  //! FreeType font instance should be already initialized!
  Standard_EXPORT bool Init(const occ::handle<OpenGl_Context>& theCtx);

  //! @return vertical distance from the horizontal baseline to the highest character coordinate
  inline float Ascender() const { return myAscender; }

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate
  inline float Descender() const { return myDescender; }

  //! Render glyph to texture if not already.
  //! @param theCtx       active context
  //! @param theUChar     unicode symbol to render
  //! @param theGlyph     computed glyph position rectangle, texture ID and UV coordinates
  Standard_EXPORT bool RenderGlyph(const occ::handle<OpenGl_Context>& theCtx,
                                   const char32_t                     theUChar,
                                   Tile&                              theGlyph);

  //! @return first texture.
  const occ::handle<OpenGl_Texture>& Texture() const { return myTextures.First(); }

protected:
  //! Render new glyph to the texture.
  bool renderGlyph(const occ::handle<OpenGl_Context>& theCtx, const char32_t theChar);

  //! Allocate new texture.
  bool createTexture(const occ::handle<OpenGl_Context>& theCtx);

protected:
  TCollection_AsciiString  myKey;        //!< key of shared resource
  occ::handle<Font_FTFont> myFont;       //!< FreeType font instance
  float                    myAscender;   //!< ascender     provided my FT font
  float                    myDescender;  //!< descender    provided my FT font
  int                      myTileSizeY;  //!< tile height
  int                      myLastTileId; //!< id of last tile
  RectI                    myLastTilePx;
  int                      myTextureFormat; //!< texture format

  NCollection_Vector<occ::handle<OpenGl_Texture>> myTextures; //!< array of textures
  NCollection_Vector<Tile>                        myTiles;    //!< array of loaded tiles

  NCollection_DataMap<char32_t, int> myGlyphMap;

public:
  DEFINE_STANDARD_RTTIEXT(OpenGl_Font, OpenGl_Resource) // Type definition
};

#endif // _OpenGl_Font_H__
