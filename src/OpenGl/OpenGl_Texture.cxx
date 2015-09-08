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

#include <OpenGl_Texture.hxx>

#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore15.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Assert.hxx>
#include <Image_PixMap.hxx>


//! Simple class to reset unpack alignment settings
struct OpenGl_UnpackAlignmentSentry
{

  //! Reset unpack alignment settings to safe values
  void Reset()
  {
    glPixelStorei (GL_UNPACK_ALIGNMENT,  1);
  #if !defined(GL_ES_VERSION_2_0)
    glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
  #endif
  }

  ~OpenGl_UnpackAlignmentSentry()
  {
    Reset();
  }

};

// =======================================================================
// function : OpenGl_Texture
// purpose  :
// =======================================================================
OpenGl_Texture::OpenGl_Texture (const Handle(Graphic3d_TextureParams)& theParams)
: OpenGl_Resource(),
  myTextureId (NO_TEXTURE),
  myTarget (GL_TEXTURE_2D),
  mySizeX (0),
  mySizeY (0),
  myTextFormat (GL_RGBA),
  myHasMipmaps (Standard_False),
  myIsAlpha    (false),
  myParams     (theParams)
{
  if (myParams.IsNull())
  {
    myParams = new Graphic3d_TextureParams();
  }
}

// =======================================================================
// function : ~OpenGl_Texture
// purpose  :
// =======================================================================
OpenGl_Texture::~OpenGl_Texture()
{
  Release (NULL);
}

// =======================================================================
// function : HasMipmaps
// purpose  :
// =======================================================================
const Standard_Boolean OpenGl_Texture::HasMipmaps() const
{
  return myHasMipmaps;
}

// =======================================================================
// function : GetParams
// purpose  :
// =======================================================================
const Handle(Graphic3d_TextureParams)& OpenGl_Texture::GetParams() const
{
  return myParams;
}

// =======================================================================
// function : SetParams
// purpose  :
// =======================================================================
void OpenGl_Texture::SetParams (const Handle(Graphic3d_TextureParams)& theParams)
{
  myParams = theParams;
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_Texture::Create (const Handle(OpenGl_Context)& )
{
  if (myTextureId == NO_TEXTURE)
  {
    glGenTextures (1, &myTextureId);
  }
  return myTextureId != NO_TEXTURE;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Texture::Release (OpenGl_Context* theGlCtx)
{
  if (myTextureId == NO_TEXTURE)
  {
    return;
  }

  // application can not handle this case by exception - this is bug in code
  Standard_ASSERT_RETURN (theGlCtx != NULL,
    "OpenGl_Texture destroyed without GL context! Possible GPU memory leakage...",);

  if (theGlCtx->IsValid())
  {
    glDeleteTextures (1, &myTextureId);
  }
  myTextureId = NO_TEXTURE;
  mySizeX = mySizeY = 0;
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void OpenGl_Texture::Bind (const Handle(OpenGl_Context)& theCtx,
                           const GLenum theTextureUnit) const
{
  if (theCtx->core15fwd != NULL)
  {
    theCtx->core15fwd->glActiveTexture (theTextureUnit);
  }
  glBindTexture (myTarget, myTextureId);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_Texture::Unbind (const Handle(OpenGl_Context)& theCtx,
                             const GLenum theTextureUnit) const
{
  if (theCtx->core15fwd != NULL)
  {
    theCtx->core15fwd->glActiveTexture (theTextureUnit);
  }
  glBindTexture (myTarget, NO_TEXTURE);
}

//=======================================================================
//function : GetDataFormat
//purpose  :
//=======================================================================
bool OpenGl_Texture::GetDataFormat (const Handle(OpenGl_Context)& theCtx,
                                    const Image_PixMap&           theData,
                                    GLint&                        theTextFormat,
                                    GLenum&                       thePixelFormat,
                                    GLenum&                       theDataType)
{
  theTextFormat  = GL_RGBA8;
  thePixelFormat = 0;
  theDataType    = 0;
  switch (theData.Format())
  {
    case Image_PixMap::ImgGrayF:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;  // GL_R32F
        thePixelFormat = GL_RED;
      }
      else
      {
        theTextFormat  = GL_LUMINANCE8;
        thePixelFormat = GL_LUMINANCE;
      }
      theDataType = GL_FLOAT;
      return true;
    }
    case Image_PixMap::ImgAlphaF:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;  // GL_R32F
        thePixelFormat = GL_RED;
      }
      else
      {
        theTextFormat  = GL_ALPHA8;
        thePixelFormat = GL_ALPHA;
      }
      theDataType = GL_FLOAT;
      return true;
    }
    case Image_PixMap::ImgRGBAF:
    {
      theTextFormat  = GL_RGBA8; // GL_RGBA32F
      thePixelFormat = GL_RGBA;
      theDataType    = GL_FLOAT;
      return true;
    }
    case Image_PixMap::ImgBGRAF:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_RGBA8;    // GL_RGBA32F
      thePixelFormat = GL_BGRA_EXT; // equals to GL_BGRA
      theDataType    = GL_FLOAT;
      return true;
    }
    case Image_PixMap::ImgRGBF:
    {
      theTextFormat  = GL_RGB8; // GL_RGB32F
      thePixelFormat = GL_RGB;
      theDataType    = GL_FLOAT;
      return true;
    }
    case Image_PixMap::ImgBGRF:
    {
    #if !defined(GL_ES_VERSION_2_0)
      theTextFormat  = GL_RGB8; // GL_RGB32F
      thePixelFormat = GL_BGR;  // equals to GL_BGR_EXT
      theDataType    = GL_FLOAT;
      return true;
    #else
      return false;
    #endif
    }
    case Image_PixMap::ImgRGBA:
    {
      theTextFormat = GL_RGBA8;
      thePixelFormat = GL_RGBA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgBGRA:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_RGBA8;
      thePixelFormat = GL_BGRA_EXT; // equals to GL_BGRA
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgRGB32:
    {
      theTextFormat = GL_RGB8;
      thePixelFormat = GL_RGBA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgBGR32:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_RGB8;
      thePixelFormat = GL_BGRA_EXT; // equals to GL_BGRA
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgRGB:
    {
      theTextFormat = GL_RGB8;
      thePixelFormat = GL_RGB;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgBGR:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      theTextFormat = GL_RGB8;
      thePixelFormat = GL_BGR; // equals to GL_BGR_EXT
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    #else
      return false;
    #endif
    }
    case Image_PixMap::ImgGray:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;
        thePixelFormat = GL_RED;
      }
      else
      {
        theTextFormat  = GL_LUMINANCE8;
        thePixelFormat = GL_LUMINANCE;
      }
      theDataType = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgAlpha:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;
        thePixelFormat = GL_RED;
      }
      else
      {
        theTextFormat  = GL_ALPHA8;
        thePixelFormat = GL_ALPHA;
      }
      theDataType = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_PixMap::ImgUNKNOWN:
    {
      return false;
    }
  }
  return false;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init (const Handle(OpenGl_Context)& theCtx,
                           const Standard_Integer        theTextFormat,
                           const GLenum                  thePixelFormat,
                           const GLenum                  theDataType,
                           const Standard_Integer        theSizeX,
                           const Standard_Integer        theSizeY,
                           const Graphic3d_TypeOfTexture theType,
                           const Image_PixMap*           theImage)
{
  if (!Create (theCtx))
  {
    Release (theCtx.operator->());
    return false;
  }

  if (theImage != NULL)
  {
    myIsAlpha = theImage->Format() == Image_PixMap::ImgAlpha
             || theImage->Format() == Image_PixMap::ImgAlphaF;
  }
  else
  {
    myIsAlpha = thePixelFormat == GL_ALPHA;
  }

  myHasMipmaps             = Standard_False;
  myTextFormat             = thePixelFormat;
#if !defined(GL_ES_VERSION_2_0)
  const GLint anIntFormat  = theTextFormat;
#else
  // ES does not support sized formats and format conversions - them detected from data type
  const GLint anIntFormat  = thePixelFormat;
#endif
  const GLsizei aWidth     = theSizeX;
  const GLsizei aHeight    = theSizeY;
  const GLsizei aMaxSize   = theCtx->MaxTextureSize();

  if (aWidth > aMaxSize || aHeight > aMaxSize)
  {
    TCollection_ExtendedString aWarnMessage = TCollection_ExtendedString ("Error: Texture dimension - ")
      + aWidth + "x" + aHeight + " exceeds hardware limits (" + aMaxSize + "x" + aMaxSize + ")";

    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);
    Release (theCtx.operator->());
    return false;
  }
#if !defined(GL_ES_VERSION_2_0)
  else if (!theCtx->IsGlGreaterEqual (3, 0) && !theCtx->arbNPTW)
  {
    // Notice that formally general NPOT textures are required by OpenGL 2.0 specifications
    // however some hardware (NV30 - GeForce FX, RadeOn 9xxx and Xxxx) supports GLSL but not NPOT!
    // Trying to create NPOT textures on such hardware will not fail
    // but driver will fall back into software rendering,
    const GLsizei aWidthP2  = OpenGl_Context::GetPowerOfTwo (aWidth, aMaxSize);
    const GLsizei aHeightP2 = OpenGl_Context::GetPowerOfTwo (aHeight, aMaxSize);

    if (aWidth != aWidthP2 || (theType != Graphic3d_TOT_1D && aHeight != aHeightP2))
    {
      TCollection_ExtendedString aWarnMessage =
        TCollection_ExtendedString ("Error: NPOT Textures (") + aWidth + "x" + aHeight + ") are not supported by hardware.";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);

      Release (theCtx.operator->());
      return false;
    }
  }
#else
  else if (!theCtx->IsGlGreaterEqual (3, 0) && theType == Graphic3d_TOT_2D_MIPMAP)
  {
    // Mipmap NPOT textures are not supported by OpenGL ES 2.0.
    const GLsizei aWidthP2  = OpenGl_Context::GetPowerOfTwo (aWidth, aMaxSize);
    const GLsizei aHeightP2 = OpenGl_Context::GetPowerOfTwo (aHeight, aMaxSize);

    if (aWidth != aWidthP2 || aHeight != aHeightP2)
    {
      TCollection_ExtendedString aWarnMessage =
        TCollection_ExtendedString ("Error: Mipmap NPOT Textures (") + aWidth + "x" + aHeight + ") are not supported by OpenGL ES 2.0";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);

      Release (theCtx.operator->());
      return false;
    }
  }
#endif

  const GLenum  aFilter   = (myParams->Filter() == Graphic3d_TOTF_NEAREST) ? GL_NEAREST : GL_LINEAR;
  const GLenum  aWrapMode = myParams->IsRepeat() ? GL_REPEAT : theCtx->TextureWrapClamp();

#if !defined(GL_ES_VERSION_2_0)
  GLint aTestWidth  = 0;
  GLint aTestHeight = 0;
#endif
  GLvoid* aDataPtr = (theImage != NULL) ? (GLvoid* )theImage->Data() : NULL;

  // setup the alignment
  OpenGl_UnpackAlignmentSentry anUnpackSentry;
  (void)anUnpackSentry; // avoid compiler warning

  if (aDataPtr != NULL)
  {
    const GLint anAligment = Min ((GLint )theImage->MaxRowAligmentBytes(), 8); // OpenGL supports alignment upto 8 bytes
    glPixelStorei (GL_UNPACK_ALIGNMENT, anAligment);

  #if !defined(GL_ES_VERSION_2_0)
    // notice that GL_UNPACK_ROW_LENGTH is not available on OpenGL ES 2.0 without GL_EXT_unpack_subimage extension
    const GLint anExtraBytes = GLint(theImage->RowExtraBytes());
    const GLint aPixelsWidth = GLint(theImage->SizeRowBytes() / theImage->SizePixelBytes());
    glPixelStorei (GL_UNPACK_ROW_LENGTH, (anExtraBytes >= anAligment) ? aPixelsWidth : 0);
  #endif
  }

  switch (theType)
  {
    case Graphic3d_TOT_1D:
    {
    #if !defined(GL_ES_VERSION_2_0)
      myTarget = GL_TEXTURE_1D;
      Bind (theCtx);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     aWrapMode);

      // use proxy to check texture could be created or not
      glTexImage1D (GL_PROXY_TEXTURE_1D, 0, anIntFormat,
                    aWidth, 0,
                    thePixelFormat, theDataType, NULL);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &aTestWidth);
      if (aTestWidth == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      glTexImage1D (GL_TEXTURE_1D, 0, anIntFormat,
                    aWidth, 0,
                    thePixelFormat, theDataType, aDataPtr);
      if (glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      mySizeX = aWidth;
      mySizeY = 1;

      Unbind (theCtx);
      return true;
    #else
      Release (theCtx.operator->());
      return false;
    #endif
    }
    case Graphic3d_TOT_2D:
    {
      myTarget = GL_TEXTURE_2D;
      Bind (theCtx);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     aWrapMode);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     aWrapMode);

    #if !defined(GL_ES_VERSION_2_0)
      // use proxy to check texture could be created or not
      glTexImage2D (GL_PROXY_TEXTURE_2D, 0, anIntFormat,
                    aWidth, aHeight, 0,
                    thePixelFormat, theDataType, NULL);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &aTestWidth);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &aTestHeight);
      if (aTestWidth == 0 || aTestHeight == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }
    #endif

      glTexImage2D (GL_TEXTURE_2D, 0, anIntFormat,
                    aWidth, aHeight, 0,
                    thePixelFormat, theDataType, aDataPtr);
      if (glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      mySizeX = aWidth;
      mySizeY = aHeight;

      Unbind (theCtx);
      return true;
    }
    case Graphic3d_TOT_2D_MIPMAP:
    {
      myTarget     = GL_TEXTURE_2D;
      myHasMipmaps = Standard_True;

      GLenum aFilterMin = aFilter;
      aFilterMin = GL_NEAREST_MIPMAP_NEAREST;
      if (myParams->Filter() == Graphic3d_TOTF_BILINEAR)
      {
        aFilterMin = GL_LINEAR_MIPMAP_NEAREST;
      }
      else if (myParams->Filter() == Graphic3d_TOTF_TRILINEAR)
      {
        aFilterMin = GL_LINEAR_MIPMAP_LINEAR;
      }

      Bind (theCtx);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterMin);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     aWrapMode);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     aWrapMode);

    #if !defined(GL_ES_VERSION_2_0)
      // use proxy to check texture could be created or not
      glTexImage2D (GL_PROXY_TEXTURE_2D, 0, anIntFormat,
                    aWidth, aHeight, 0,
                    thePixelFormat, theDataType, NULL);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &aTestWidth);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &aTestHeight);
      if (aTestWidth == 0 || aTestHeight == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }
    #endif

      // upload main picture
      glTexImage2D (GL_TEXTURE_2D, 0, anIntFormat,
                    aWidth, aHeight, 0,
                    thePixelFormat, theDataType, theImage->Data());
      if (glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      mySizeX = aWidth;
      mySizeY = aHeight;

      if (theCtx->arbFBO != NULL)
      {
        // generate mipmaps
        //glHint (GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        theCtx->arbFBO->glGenerateMipmap (GL_TEXTURE_2D);

        if (glGetError() != GL_NO_ERROR)
        {
          Unbind (theCtx);
          Release (theCtx.operator->());
          return false;
        }
      }
      else
      {
        const TCollection_ExtendedString aWarnMessage ("Warning: generating mipmaps requires GL_ARB_framebuffer_object extension which is missing.");

        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_PORTABILITY_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, aWarnMessage);

        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      Unbind (theCtx);
      return true;
    }
    default:
    {
      Release (theCtx.operator->());
      return false;
    }
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init (const Handle(OpenGl_Context)& theCtx,
                           const Image_PixMap&           theImage,
                           const Graphic3d_TypeOfTexture theType)
{
  if (theImage.IsEmpty())
  {
    Release (theCtx.operator->());
    return false;
  }

  GLenum aPixelFormat;
  GLenum aDataType;
  GLint aTextFormat;
  if (!GetDataFormat (theCtx, theImage, aTextFormat, aPixelFormat, aDataType))
  {
    Release (theCtx.operator->());
    return false;
  }

  return Init (theCtx,
               aTextFormat, aPixelFormat, aDataType,
               (Standard_Integer)theImage.SizeX(),
               (Standard_Integer)theImage.SizeY(),
               theType, &theImage);
}

// =======================================================================
// function : InitRectangle
// purpose  :
// =======================================================================
bool OpenGl_Texture::InitRectangle (const Handle(OpenGl_Context)& theCtx,
                                    const Standard_Integer        theSizeX,
                                    const Standard_Integer        theSizeY,
                                    const OpenGl_TextureFormat&   theFormat)
{
  if (!Create (theCtx) || !theCtx->IsGlGreaterEqual (3, 0))
  {
    return false;
  }

#if !defined(GL_ES_VERSION_2_0)
  myTarget = GL_TEXTURE_RECTANGLE;

  const GLsizei aSizeX    = Min (theCtx->MaxTextureSize(), theSizeX);
  const GLsizei aSizeY    = Min (theCtx->MaxTextureSize(), theSizeY);
  const GLenum  aFilter   = (myParams->Filter() == Graphic3d_TOTF_NEAREST) ? GL_NEAREST : GL_LINEAR;
  const GLenum  aWrapMode = myParams->IsRepeat() ? GL_REPEAT : theCtx->TextureWrapClamp();

  Bind (theCtx);
  glTexParameteri (myTarget, GL_TEXTURE_MIN_FILTER, aFilter);
  glTexParameteri (myTarget, GL_TEXTURE_MAG_FILTER, aFilter);
  glTexParameteri (myTarget, GL_TEXTURE_WRAP_S,     aWrapMode);
  glTexParameteri (myTarget, GL_TEXTURE_WRAP_T,     aWrapMode);

  const GLint anIntFormat = theFormat.Internal();
  myTextFormat = theFormat.Format();

  glTexImage2D (GL_PROXY_TEXTURE_RECTANGLE,
                0,
                anIntFormat,
                aSizeX,
                aSizeY,
                0,
                theFormat.Format(),
                GL_FLOAT,
                NULL);

  GLint aTestSizeX = 0;
  GLint aTestSizeY = 0;

  glGetTexLevelParameteriv (
    GL_PROXY_TEXTURE_RECTANGLE, 0, GL_TEXTURE_WIDTH,  &aTestSizeX);
  glGetTexLevelParameteriv (
    GL_PROXY_TEXTURE_RECTANGLE, 0, GL_TEXTURE_HEIGHT, &aTestSizeY);

  if (aTestSizeX == 0 || aTestSizeY == 0)
  {
    Unbind (theCtx);
    return false;
  }

  glTexImage2D (myTarget,
                0,
                anIntFormat,
                aSizeX,
                aSizeY,
                0,
                theFormat.Format(),
                GL_FLOAT,
                NULL);

  if (glGetError() != GL_NO_ERROR)
  {
    Unbind (theCtx);
    return false;
  }

  mySizeX = aSizeX;
  mySizeY = aSizeY;

  Unbind (theCtx);
  return true;
#else
  return false;
#endif
}
