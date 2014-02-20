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

#include <OpenGl_ExtFBO.hxx>
#include <OpenGl_Context.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Standard_Assert.hxx>
#include <Image_PixMap.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_Texture, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Texture, OpenGl_Resource)

//! Simple class to reset unpack alignment settings
struct OpenGl_UnpackAlignmentSentry
{

  //! Reset unpack alignment settings to safe values
  void Reset()
  {
    glPixelStorei (GL_UNPACK_ALIGNMENT,  1);
    glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
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
  myTextFormat (GL_FLOAT),
  myHasMipmaps (Standard_False),
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
void OpenGl_Texture::Release (const OpenGl_Context* theGlCtx)
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
  if (theCtx->IsGlGreaterEqual (1, 3))
  {
    theCtx->core13->glActiveTexture (theTextureUnit);
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
  if (theCtx->IsGlGreaterEqual (1, 3))
  {
    theCtx->core13->glActiveTexture (theTextureUnit);
  }
  glBindTexture (myTarget, NO_TEXTURE);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init (const Handle(OpenGl_Context)& theCtx,
                           const Image_PixMap&           theImage,
                           const Graphic3d_TypeOfTexture theType)
{
  myHasMipmaps = Standard_False;
  if (theImage.IsEmpty() || !Create (theCtx))
  {
    return false;
  }

  myTextFormat = GL_RGBA8;
  GLenum aPixelFormat = 0;
  GLenum aDataType    = 0;
  switch (theImage.Format())
  {
    case Image_PixMap::ImgGrayF:
    {
      myTextFormat = GL_ALPHA8; // GL_R8, GL_R32F
      aPixelFormat = GL_ALPHA;  // GL_RED
      aDataType    = GL_FLOAT;
      break;
    }
    case Image_PixMap::ImgRGBAF:
    {
      myTextFormat = GL_RGBA8; // GL_RGBA32F
      aPixelFormat = GL_RGBA;
      aDataType    = GL_FLOAT;
      break;
    }
    case Image_PixMap::ImgBGRAF:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      myTextFormat = GL_RGBA8; // GL_RGBA32F
      aPixelFormat = GL_BGRA;  // equals to GL_BGRA_EXT
      aDataType    = GL_FLOAT;
      break;
    }
    case Image_PixMap::ImgRGBF:
    {
      myTextFormat = GL_RGB8; // GL_RGB32F
      aPixelFormat = GL_RGB;
      aDataType    = GL_FLOAT;
      break;
    }
    case Image_PixMap::ImgBGRF:
    {
      myTextFormat = GL_RGB8; // GL_RGB32F
      aPixelFormat = GL_BGR;  // equals to GL_BGR_EXT
      aDataType    = GL_FLOAT;
      break;
    }
    case Image_PixMap::ImgRGBA:
    {
      myTextFormat = GL_RGBA8;
      aPixelFormat = GL_RGBA;
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_PixMap::ImgBGRA:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      myTextFormat = GL_RGBA8;
      aPixelFormat = GL_BGRA;  // equals to GL_BGRA_EXT
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_PixMap::ImgRGB32:
    {
      myTextFormat = GL_RGB8;
      aPixelFormat = GL_RGBA;
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_PixMap::ImgBGR32:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      myTextFormat = GL_RGB8;
      aPixelFormat = GL_BGRA;  // equals to GL_BGRA_EXT
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_PixMap::ImgRGB:
    {
      myTextFormat = GL_RGB8;
      aPixelFormat = GL_RGB;
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_PixMap::ImgBGR:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      myTextFormat = GL_RGB8;
      aPixelFormat = GL_BGR;  // equals to GL_BGR_EXT
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_PixMap::ImgGray:
    {
      myTextFormat = GL_ALPHA8; // GL_R8
      aPixelFormat = GL_ALPHA;  // GL_RED
      aDataType    = GL_UNSIGNED_BYTE;
      break;
    }
    default:
    {
      return false;
    }
  }

  const GLsizei aMaxSize   = theCtx->MaxTextureSize();
  const GLsizei aWidth     = (GLsizei )theImage.SizeX();
  const GLsizei aHeight    = (GLsizei )theImage.SizeY();

  // Notice that formally general NPOT textures are required by OpenGL 2.0 specifications
  // however some hardware (NV30 - GeForce FX, RadeOn 9xxx and Xxxx) supports GLSL but not NPOT!
  // Trying to create NPOT rextures on such hardware will not fail
  // but driver will fall back into software rendering,
  const bool    toForceP2  = !theCtx->IsGlGreaterEqual (3, 0) && !theCtx->arbNPTW;
  const GLsizei aWidthOut  = toForceP2 ? OpenGl_Context::GetPowerOfTwo (aWidth,  aMaxSize) : Min (aWidth,  aMaxSize);
  const GLsizei aHeightOut = toForceP2 ? OpenGl_Context::GetPowerOfTwo (aHeight, aMaxSize) : Min (aHeight, aMaxSize);

  GLint aTestWidth  = 0;
  GLint aTestHeight = 0;

  // setup the alignment
  OpenGl_UnpackAlignmentSentry anUnpackSentry;
  const GLint anAligment = Min ((GLint )theImage.MaxRowAligmentBytes(), 8); // OpenGL supports alignment upto 8 bytes
  glPixelStorei (GL_UNPACK_ALIGNMENT, anAligment);

  // notice that GL_UNPACK_ROW_LENGTH is not available on OpenGL ES 2.0 without GL_EXT_unpack_subimage extension
  const GLint anExtraBytes = GLint(theImage.RowExtraBytes());
  const GLint aPixelsWidth = GLint(theImage.SizeRowBytes() / theImage.SizePixelBytes());
  glPixelStorei (GL_UNPACK_ROW_LENGTH, (anExtraBytes >= anAligment) ? aPixelsWidth : 0);

  switch (theType)
  {
    case Graphic3d_TOT_1D:
    {
      myTarget = GL_TEXTURE_1D;
      Bind (theCtx);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      Image_PixMap aCopy;
      GLvoid* aDataPtr = (GLvoid* )theImage.Data();
      if (aWidth != aWidthOut)
      {
        glPixelStorei (GL_PACK_ALIGNMENT,  1);
        glPixelStorei (GL_PACK_ROW_LENGTH, 0);
        if (!aCopy.InitTrash (theImage.Format(), Standard_Size(aWidthOut), 1)
          || gluScaleImage (aPixelFormat,
                            aWidth,    1, aDataType, theImage.Data(),
                            aWidthOut, 1, aDataType, aCopy.ChangeData()) != 0)
        {
          Unbind (theCtx);
          return false;
        }

        aDataPtr = (GLvoid* )aCopy.Data();
        anUnpackSentry.Reset();
      }

      // use proxy to check texture could be created or not
      glTexImage1D (GL_PROXY_TEXTURE_1D, 0, myTextFormat,
                    aWidthOut, 0,
                    aPixelFormat, aDataType, NULL);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &aTestWidth);
      if (aTestWidth == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        return false;
      }

      glTexImage1D (GL_TEXTURE_1D, 0, myTextFormat,
                    aWidthOut, 0,
                    aPixelFormat, aDataType, aDataPtr);
      if (glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        return false;
      }

      mySizeX = aWidthOut;
      mySizeY = 1;

      Unbind (theCtx);
      return true;
    }
    case Graphic3d_TOT_2D:
    {
      myTarget = GL_TEXTURE_2D;
      Bind (theCtx);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Image_PixMap aCopy;
      GLvoid* aDataPtr = (GLvoid* )theImage.Data();
      if (aWidth != aWidthOut || aHeight != aHeightOut)
      {
        // scale texture
        glPixelStorei (GL_PACK_ALIGNMENT,  1);
        glPixelStorei (GL_PACK_ROW_LENGTH, 0);
        if (!aCopy.InitTrash (theImage.Format(), Standard_Size(aWidthOut), Standard_Size(aHeightOut))
          || gluScaleImage (aPixelFormat,
                            aWidth,    aHeight,    aDataType, theImage.Data(),
                            aWidthOut, aHeightOut, aDataType, aCopy.ChangeData()) != 0)
        {
          Unbind (theCtx);
          return false;
        }

        aDataPtr = (GLvoid* )aCopy.Data();
        anUnpackSentry.Reset();
      }

      // use proxy to check texture could be created or not
      glTexImage2D (GL_PROXY_TEXTURE_2D, 0, myTextFormat,
                    aWidthOut, aHeightOut, 0,
                    aPixelFormat, aDataType, NULL);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &aTestWidth);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &aTestHeight);
      if (aTestWidth == 0 || aTestHeight == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        return false;
      }

      glTexImage2D (GL_TEXTURE_2D, 0, myTextFormat,
                    aWidthOut, aHeightOut, 0,
                    aPixelFormat, aDataType, aDataPtr);
      if (glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        return false;
      }

      mySizeX = aWidthOut;
      mySizeY = aHeightOut;

      Unbind (theCtx);
      return true;
    }
    case Graphic3d_TOT_2D_MIPMAP:
    {
      myTarget     = GL_TEXTURE_2D;
      myHasMipmaps = Standard_True;
      Bind (theCtx);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if (theCtx->extFBO != NULL
       && aWidth == aWidthOut && aHeight == aHeightOut)
      {
        // use proxy to check texture could be created or not
        glTexImage2D (GL_PROXY_TEXTURE_2D, 0, myTextFormat,
                      aWidthOut, aHeightOut, 0,
                      aPixelFormat, aDataType, NULL);
        glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &aTestWidth);
        glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &aTestHeight);
        if (aTestWidth == 0 || aTestHeight == 0)
        {
          // no memory or broken input parameters
          Unbind (theCtx);
          return false;
        }

        // upload main picture
        glTexImage2D (GL_TEXTURE_2D, 0, myTextFormat,
                      aWidthOut, aHeightOut, 0,
                      aPixelFormat, aDataType, theImage.Data());
        if (glGetError() != GL_NO_ERROR)
        {
          Unbind (theCtx);
          return false;
        }

        mySizeX = aWidthOut;
        mySizeY = aHeightOut;

        // generate mipmaps
        //glHint (GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        theCtx->extFBO->glGenerateMipmapEXT (GL_TEXTURE_2D);

        Unbind (theCtx);
        return true;
      }
      else
      {
        bool isCreated = gluBuild2DMipmaps (GL_TEXTURE_2D, myTextFormat,
                                            aWidth, aHeight,
                                            aPixelFormat, aDataType, theImage.Data()) == 0;
        if (isCreated)
        {
          glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &mySizeX);
          glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &mySizeY);
        }

        Unbind (theCtx);
        return isCreated;
      }
    }
    default:
    {
      return false;
    }
  }
}
