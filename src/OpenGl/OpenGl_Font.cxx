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

#include <OpenGl_Font.hxx>

#include <OpenGl_Context.hxx>
#include <Font_FTFont.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Font, OpenGl_Resource)

// =======================================================================
// function : OpenGl_Font
// purpose  :
// =======================================================================
OpenGl_Font::OpenGl_Font(const Handle(Font_FTFont)& theFont, const TCollection_AsciiString& theKey)
    : myKey(theKey),
      myFont(theFont),
      myAscender(0.0f),
      myDescender(0.0f),
      myTileSizeY(0),
      myLastTileId(-1),
      myTextureFormat(GL_ALPHA)
{
  memset(&myLastTilePx, 0, sizeof(myLastTilePx));
}

// =======================================================================
// function : ~OpenGl_Font
// purpose  :
// =======================================================================
OpenGl_Font::~OpenGl_Font()
{
  Release(NULL);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Font::Release(OpenGl_Context* theCtx)
{
  if (myTextures.IsEmpty())
  {
    return;
  }

  for (Standard_Integer anIter = 0; anIter < myTextures.Length(); ++anIter)
  {
    Handle(OpenGl_Texture)& aTexture = myTextures.ChangeValue(anIter);
    if (aTexture->IsValid())
    {
      // application can not handle this case by exception - this is bug in code
      Standard_ASSERT_RETURN(
        theCtx != NULL,
        "OpenGl_Font destroyed without GL context! Possible GPU memory leakage...", );
    }

    aTexture->Release(theCtx);
    aTexture.Nullify();
  }
  myTextures.Clear();
}

// =======================================================================
// function : EstimatedDataSize
// purpose  :
// =======================================================================
Standard_Size OpenGl_Font::EstimatedDataSize() const
{
  Standard_Size aSize = 0;
  for (NCollection_Vector<Handle(OpenGl_Texture)>::Iterator aTexIter(myTextures); aTexIter.More();
       aTexIter.Next())
  {
    aSize += aTexIter.Value()->EstimatedDataSize();
  }
  return aSize;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Font::Init(const Handle(OpenGl_Context)& theCtx)
{
  Release(theCtx.operator->());
  if (myFont.IsNull() || !myFont->IsValid())
  {
    return false;
  }

  myAscender  = myFont->Ascender();
  myDescender = myFont->Descender();
  myTileSizeY = myFont->GlyphMaxSizeY(true);

  myLastTileId = -1;
  if (!createTexture(theCtx))
  {
    Release(theCtx.operator->());
    return false;
  }
  return true;
}

// =======================================================================
// function : createTexture
// purpose  :
// =======================================================================
bool OpenGl_Font::createTexture(const Handle(OpenGl_Context)& theCtx)
{
  // Single font might define very wide range of symbols, with very few of them actually used in
  // text. Limit single texture with circa 4096 glyphs.
  static const Standard_Integer THE_MAX_GLYPHS_PER_TEXTURE = 4096;

  myTileSizeY = myFont->GlyphMaxSizeY(true);
  const Standard_Integer aGlyphsNb =
    Min(THE_MAX_GLYPHS_PER_TEXTURE, myFont->GlyphsNumber(true) - myLastTileId + 1);
  const Standard_Integer aMaxTileSizeX = myFont->GlyphMaxSizeX(true);
  const Standard_Integer aMaxSize      = theCtx->MaxTextureSize();
  const Standard_Integer aTextureSizeX =
    OpenGl_Context::GetPowerOfTwo(aGlyphsNb * aMaxTileSizeX, aMaxSize);
  const Standard_Integer aTilesPerRow = aTextureSizeX / aMaxTileSizeX;
  const Standard_Integer aTextureSizeY =
    OpenGl_Context::GetPowerOfTwo(GLint((aGlyphsNb / aTilesPerRow) + 1) * myTileSizeY, aMaxSize);

  memset(&myLastTilePx, 0, sizeof(myLastTilePx));
  myLastTilePx.Bottom = myTileSizeY;

  Handle(Graphic3d_TextureParams) aParams = new Graphic3d_TextureParams();
  aParams->SetModulate(Standard_False);
  aParams->SetRepeat(Standard_False);
  aParams->SetFilter(Graphic3d_TOTF_BILINEAR);
  aParams->SetAnisoFilter(Graphic3d_LOTA_OFF);

  myTextures.Append(new OpenGl_Texture(myKey + "_texture" + myTextures.Size(), aParams));
  Handle(OpenGl_Texture)& aTexture = myTextures.ChangeLast();

  Image_PixMap aBlackImg;
  if (!aBlackImg.InitZero(Image_Format_Alpha,
                          Standard_Size(aTextureSizeX),
                          Standard_Size(aTextureSizeY))
      || !aTexture->Init(theCtx, aBlackImg, Graphic3d_TypeOfTexture_2D, true)) // myTextureFormat
  {
    theCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        TCollection_AsciiString("New texture initialization of size ")
                          + aTextureSizeX + "x" + aTextureSizeY + " for textured font has failed.");
    return false;
  }

  return true;
}

// =======================================================================
// function : renderGlyph
// purpose  :
// =======================================================================
bool OpenGl_Font::renderGlyph(const Handle(OpenGl_Context)& theCtx,
                              const Standard_Utf32Char      theChar)
{
  if (!myFont->RenderGlyph(theChar))
  {
    return false;
  }

  Handle(OpenGl_Texture)& aTexture = myTextures.ChangeLast();
  if (aTexture.IsNull() || !aTexture->IsValid())
  {
    return false;
  }

  const Image_PixMap&    anImg   = myFont->GlyphImage();
  const Standard_Integer aTileId = myLastTileId + 1;
  myLastTilePx.Left              = myLastTilePx.Right + 3;
  myLastTilePx.Right             = myLastTilePx.Left + (Standard_Integer)anImg.SizeX();
  if (myLastTilePx.Right > aTexture->SizeX() || (Standard_Integer)anImg.SizeY() > myTileSizeY)
  {
    myTileSizeY = myFont->GlyphMaxSizeY(true);

    myLastTilePx.Left  = 0;
    myLastTilePx.Right = (Standard_Integer)anImg.SizeX();
    myLastTilePx.Top += myTileSizeY;
    myLastTilePx.Bottom += myTileSizeY;

    if (myLastTilePx.Bottom > aTexture->SizeY() || myLastTilePx.Right > aTexture->SizeX())
    {
      if (!createTexture(theCtx))
      {
        return false;
      }
      return renderGlyph(theCtx, theChar);
    }
  }

  aTexture->Bind(theCtx);
  if (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
  {
    theCtx->core11fwd->glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
  }
  if (theCtx->hasUnpackRowLength)
  {
    theCtx->core11fwd->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  }
  theCtx->core11fwd->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  theCtx->core11fwd->glTexSubImage2D(GL_TEXTURE_2D,
                                     0,
                                     myLastTilePx.Left,
                                     myLastTilePx.Top,
                                     (GLsizei)anImg.SizeX(),
                                     (GLsizei)anImg.SizeY(),
                                     aTexture->GetFormat(),
                                     GL_UNSIGNED_BYTE,
                                     anImg.Data());

  OpenGl_Font::Tile aTile;
  aTile.uv.Left   = GLfloat(myLastTilePx.Left) / GLfloat(aTexture->SizeX());
  aTile.uv.Right  = GLfloat(myLastTilePx.Right) / GLfloat(aTexture->SizeX());
  aTile.uv.Top    = GLfloat(myLastTilePx.Top) / GLfloat(aTexture->SizeY());
  aTile.uv.Bottom = GLfloat(myLastTilePx.Top + anImg.SizeY()) / GLfloat(aTexture->SizeY());
  aTile.texture   = aTexture->TextureId();
  myFont->GlyphRect(aTile.px);

  myLastTileId = aTileId;
  myTiles.Append(aTile);
  return true;
}

// =======================================================================
// function : RenderGlyph
// purpose  :
// =======================================================================
bool OpenGl_Font::RenderGlyph(const Handle(OpenGl_Context)& theCtx,
                              const Standard_Utf32Char      theUChar,
                              Tile&                         theGlyph)
{
  Standard_Integer aTileId = 0;
  if (!myGlyphMap.Find(theUChar, aTileId))
  {
    if (renderGlyph(theCtx, theUChar))
    {
      aTileId = myLastTileId;
    }
    else
    {
      return false;
    }

    myGlyphMap.Bind(theUChar, aTileId);
  }

  const OpenGl_Font::Tile& aTile = myTiles.Value(aTileId);
  theGlyph.px                    = aTile.px;
  theGlyph.uv                    = aTile.uv;
  theGlyph.texture               = aTile.texture;

  return true;
}
