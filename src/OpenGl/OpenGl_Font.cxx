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

#include <OpenGl_Font.hxx>

#include <OpenGl_Context.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_Font, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Font, OpenGl_Resource)

// =======================================================================
// function : OpenGl_Font
// purpose  :
// =======================================================================
OpenGl_Font::OpenGl_Font (const Handle(Font_FTFont)&     theFont,
                          const TCollection_AsciiString& theKey)
: myKey  (theKey),
  myFont (theFont),
  myAscender (0.0f),
  myDescender (0.0f),
  myLineSpacing (0.0f),
  myTileSizeX (0),
  myTileSizeY (0),
  myLastTileId (-1),
  myTextureFormat (GL_ALPHA8)
{
  memset (&myLastTilePx, 0, sizeof(myLastTilePx));
}

// =======================================================================
// function : ~OpenGl_Font
// purpose  :
// =======================================================================
OpenGl_Font::~OpenGl_Font()
{
  Release (NULL);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Font::Release (const OpenGl_Context* theCtx)
{
  if (myTextures.IsEmpty())
  {
    return;
  }

  // application can not handle this case by exception - this is bug in code
  Standard_ASSERT_RETURN (theCtx != NULL,
    "OpenGl_Font destroyed without GL context! Possible GPU memory leakage...",);

  for (Standard_Integer anIter = 0; anIter < myTextures.Length(); ++anIter)
  {
    Handle(OpenGl_Texture)& aTexture = myTextures.ChangeValue (anIter);
    aTexture->Release (theCtx);
    aTexture.Nullify();
  }
  myTextures.Clear();
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Font::Init (const Handle(OpenGl_Context)& theCtx)
{
  Release (theCtx.operator->());
  if (myFont.IsNull() || !myFont->IsValid())
  {
    return false;
  }

  myAscender    = myFont->Ascender();
  myDescender   = myFont->Descender();
  myLineSpacing = myFont->LineSpacing();
  myTileSizeX   = myFont->GlyphMaxSizeX();
  myTileSizeY   = myFont->GlyphMaxSizeY();

  myLastTileId = -1;
  return createTexture (theCtx);
}

// =======================================================================
// function : createTexture
// purpose  :
// =======================================================================
bool OpenGl_Font::createTexture (const Handle(OpenGl_Context)& theCtx)
{
  const Standard_Integer aMaxSize = theCtx->MaxTextureSize();

  Standard_Integer aGlyphsNb = myFont->GlyphsNumber() - myLastTileId + 1;

  const Standard_Integer aTextureSizeX = OpenGl_Context::GetPowerOfTwo (aGlyphsNb * myTileSizeX, aMaxSize);
  const Standard_Integer aTilesPerRow  = aTextureSizeX / myTileSizeX;
  const Standard_Integer aTextureSizeY = OpenGl_Context::GetPowerOfTwo (GLint((aGlyphsNb / aTilesPerRow) + 1) * myTileSizeY, aMaxSize);

  memset (&myLastTilePx, 0, sizeof(myLastTilePx));
  myLastTilePx.Bottom = myTileSizeY;

  myTextures.Append (new OpenGl_Texture());
  Handle(OpenGl_Texture)& aTexture = myTextures.ChangeLast();

  Image_PixMap aBlackImg;
  if (!aBlackImg.InitZero (Image_PixMap::ImgGray, Standard_Size(aTextureSizeX), Standard_Size(aTextureSizeY))
   || !aTexture->Init (theCtx, aBlackImg, Graphic3d_TOT_2D)) // myTextureFormat
  {
    return false;
  }

  aTexture->Bind (theCtx);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  aTexture->Unbind (theCtx);
  return true;
}

// =======================================================================
// function : renderGlyph
// purpose  :
// =======================================================================
bool OpenGl_Font::renderGlyph (const Handle(OpenGl_Context)& theCtx,
                               const Standard_Utf32Char      theChar)
{
  if (!myFont->RenderGlyph (theChar))
  {
    return false;
  }

  Handle(OpenGl_Texture)& aTexture = myTextures.ChangeLast();

  const Image_PixMap& anImg = myFont->GlyphImage();
  const Standard_Integer aTileId = myLastTileId + 1;
  myLastTilePx.Left  = myLastTilePx.Right + 3;
  myLastTilePx.Right = myLastTilePx.Left + (Standard_Integer )anImg.SizeX();
  if (myLastTilePx.Right >= aTexture->SizeX())
  {
    myLastTilePx.Left    = 0;
    myLastTilePx.Right   = (Standard_Integer )anImg.SizeX();
    myLastTilePx.Top    += myTileSizeY;
    myLastTilePx.Bottom += myTileSizeY;

    if (myLastTilePx.Bottom >= aTexture->SizeY())
    {
      if (!createTexture (theCtx))
      {
        return false;
      }
      return renderGlyph (theCtx, theChar);
    }
  }

  aTexture->Bind (theCtx);
  glPixelStorei (GL_UNPACK_LSB_FIRST,  GL_FALSE);
  glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei (GL_UNPACK_ALIGNMENT,  1);

  glTexSubImage2D (GL_TEXTURE_2D, 0,
                   myLastTilePx.Left, myLastTilePx.Top, (GLsizei )anImg.SizeX(), (GLsizei )anImg.SizeY(),
                   GL_ALPHA, GL_UNSIGNED_BYTE, anImg.Data());

  OpenGl_Font::Tile aTile;
  aTile.uv.Left   = GLfloat(myLastTilePx.Left)                / GLfloat(aTexture->SizeX());
  aTile.uv.Right  = GLfloat(myLastTilePx.Right)               / GLfloat(aTexture->SizeX());
  aTile.uv.Top    = GLfloat(myLastTilePx.Top)                 / GLfloat(aTexture->SizeY());
  aTile.uv.Bottom = GLfloat(myLastTilePx.Top + anImg.SizeY()) / GLfloat(aTexture->SizeY());
  aTile.texture   = aTexture->TextureId();
  myFont->GlyphRect (aTile.px);

  myLastTileId = aTileId;
  myTiles.Append (aTile);
  return true;
}

// =======================================================================
// function : RenderGlyph
// purpose  :
// =======================================================================
void OpenGl_Font::RenderGlyph (const Handle(OpenGl_Context)& theCtx,
                               const Standard_Utf32Char      theUChar,
                               const Standard_Utf32Char      theUCharNext,
                               OpenGl_Font::Tile&            theGlyph,
                               OpenGl_Vec2&                  thePen)
{
  Standard_Integer aTileId = 0;
  if (!myGlyphMap.Find (theUChar, aTileId))
  {
    if (renderGlyph (theCtx, theUChar))
    {
      aTileId = myLastTileId;
    }
    else
    {
      thePen.x() += myFont->AdvanceX (theUChar, theUCharNext);
      return;
    }
    myGlyphMap.Bind (theUChar, aTileId);
  }

  const OpenGl_Font::Tile& aTile = myTiles.Value (aTileId);
  theGlyph.px.Top    = thePen.y() + aTile.px.Top;
  theGlyph.px.Bottom = thePen.y() + aTile.px.Bottom;
  theGlyph.px.Left   = thePen.x() + aTile.px.Left;
  theGlyph.px.Right  = thePen.x() + aTile.px.Right;
  theGlyph.uv        = aTile.uv;
  theGlyph.texture   = aTile.texture;

  thePen.x() += myFont->AdvanceX (theUChar, theUCharNext);
}
