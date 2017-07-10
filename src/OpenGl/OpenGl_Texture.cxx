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
#include <OpenGl_GlCore32.hxx>
#include <OpenGl_Sampler.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Assert.hxx>
#include <Image_PixMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Texture, OpenGl_NamedResource)

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
OpenGl_Texture::OpenGl_Texture (const TCollection_AsciiString& theResourceId,
                                const Handle(Graphic3d_TextureParams)& theParams)
: OpenGl_NamedResource (theResourceId),
  mySampler (new OpenGl_Sampler (theParams)),
  myRevision (0),
  myTextureId (NO_TEXTURE),
  myTarget (GL_TEXTURE_2D),
  mySizeX (0),
  mySizeY (0),
  mySizeZ (0),
  myTextFormat (GL_RGBA),
  myHasMipmaps (Standard_False),
  myIsAlpha    (false)
{
  //
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
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_Texture::Create (const Handle(OpenGl_Context)& theCtx)
{
  if (myTextureId != NO_TEXTURE)
  {
    return true;
  }

  theCtx->core11fwd->glGenTextures (1, &myTextureId);
  if (myTextureId == NO_TEXTURE)
  {
    return false;
  }

  //mySampler->Create (theCtx); // do not create sampler object by default
  return true;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Texture::Release (OpenGl_Context* theGlCtx)
{
  mySampler->Release (theGlCtx);
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
  mySizeX = mySizeY = mySizeZ = 0;
}

// =======================================================================
// function : applyDefaultSamplerParams
// purpose  :
// =======================================================================
void OpenGl_Texture::applyDefaultSamplerParams (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Sampler::applySamplerParams (theCtx, mySampler->Parameters(), NULL, myTarget, myHasMipmaps);
  if (mySampler->IsValid() && !mySampler->IsImmutable())
  {
    OpenGl_Sampler::applySamplerParams (theCtx, mySampler->Parameters(), mySampler.get(), myTarget, myHasMipmaps);
  }
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void OpenGl_Texture::Bind (const Handle(OpenGl_Context)& theCtx,
                           const Graphic3d_TextureUnit   theTextureUnit) const
{
  if (theCtx->core15fwd != NULL)
  {
    theCtx->core15fwd->glActiveTexture (GL_TEXTURE0 + theTextureUnit);
  }
  mySampler->Bind (theCtx, theTextureUnit);
  glBindTexture (myTarget, myTextureId);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_Texture::Unbind (const Handle(OpenGl_Context)& theCtx,
                             const Graphic3d_TextureUnit   theTextureUnit) const
{
  if (theCtx->core15fwd != NULL)
  {
    theCtx->core15fwd->glActiveTexture (GL_TEXTURE0 + theTextureUnit);
  }
  mySampler->Unbind (theCtx, theTextureUnit);
  glBindTexture (myTarget, NO_TEXTURE);
}

//=======================================================================
//function : InitSamplerObject
//purpose  :
//=======================================================================
bool OpenGl_Texture::InitSamplerObject (const Handle(OpenGl_Context)& theCtx)
{
  return myTextureId != NO_TEXTURE
      && mySampler->Init (theCtx, *this);
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
    case Image_Format_GrayF:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;  // GL_R32F
        thePixelFormat = GL_RED;
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        theTextFormat  = GL_LUMINANCE8;
      #else
        theTextFormat  = GL_LUMINANCE;
      #endif
        thePixelFormat = GL_LUMINANCE;
      }
      theDataType = GL_FLOAT;
      return true;
    }
    case Image_Format_AlphaF:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;  // GL_R32F
        thePixelFormat = GL_RED;
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        theTextFormat  = GL_ALPHA8;
      #else
        theTextFormat  = GL_ALPHA;
      #endif
        thePixelFormat = GL_ALPHA;
      }
      theDataType = GL_FLOAT;
      return true;
    }
    case Image_Format_RGBAF:
    {
      theTextFormat  = GL_RGBA8; // GL_RGBA32F
      thePixelFormat = GL_RGBA;
      theDataType    = GL_FLOAT;
      return true;
    }
    case Image_Format_BGRAF:
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
    case Image_Format_RGBF:
    {
      theTextFormat  = GL_RGB8; // GL_RGB32F
      thePixelFormat = GL_RGB;
      theDataType    = GL_FLOAT;
      return true;
    }
    case Image_Format_BGRF:
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
    case Image_Format_RGBA:
    {
      theTextFormat = GL_RGBA8;
      thePixelFormat = GL_RGBA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_BGRA:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_RGBA8;
    #else
      if (!theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_BGRA_EXT;
    #endif
      thePixelFormat = GL_BGRA_EXT; // equals to GL_BGRA
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_RGB32:
    {
    #if !defined(GL_ES_VERSION_2_0)
      // ask driver to convert data to RGB8 to save memory
      theTextFormat  = GL_RGB8;
    #else
      // conversion is not supported
      theTextFormat  = GL_RGBA8;
    #endif
      thePixelFormat = GL_RGBA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_BGR32:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (!theCtx->IsGlGreaterEqual(1, 2) && !theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_RGB8;
    #else
      if (!theCtx->extBgra)
      {
        return false;
      }
      theTextFormat  = GL_BGRA_EXT;
    #endif
      thePixelFormat = GL_BGRA_EXT; // equals to GL_BGRA
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_RGB:
    {
      theTextFormat = GL_RGB8;
      thePixelFormat = GL_RGB;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_BGR:
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
    case Image_Format_Gray:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;
        thePixelFormat = GL_RED;
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        theTextFormat  = GL_LUMINANCE8;
      #else
        theTextFormat  = GL_LUMINANCE;
      #endif
        thePixelFormat = GL_LUMINANCE;
      }
      theDataType = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_Alpha:
    {
      if (theCtx->core11 == NULL)
      {
        theTextFormat  = GL_R8;
        thePixelFormat = GL_RED;
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        theTextFormat  = GL_ALPHA8;
      #else
        theTextFormat  = GL_ALPHA;
      #endif
        thePixelFormat = GL_ALPHA;
      }
      theDataType = GL_UNSIGNED_BYTE;
      return true;
    }
    case Image_Format_UNKNOWN:
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
#if !defined(GL_ES_VERSION_2_0)
  const GLenum aTarget = theType == Graphic3d_TOT_1D
                       ? GL_TEXTURE_1D
                       : GL_TEXTURE_2D;
#else
  const GLenum aTarget = GL_TEXTURE_2D;
#endif
  const Standard_Boolean toCreateMipMaps = (theType == Graphic3d_TOT_2D_MIPMAP);
  const bool toPatchExisting = IsValid()
                            && myTextFormat == thePixelFormat
                            && myTarget == aTarget
                            && myHasMipmaps == toCreateMipMaps
                            && mySizeX  == theSizeX
                            && (mySizeY == theSizeY || theType == Graphic3d_TOT_1D);
  if (!Create (theCtx))
  {
    Release (theCtx.operator->());
    return false;
  }

  if (theImage != NULL)
  {
    myIsAlpha = theImage->Format() == Image_Format_Alpha
             || theImage->Format() == Image_Format_AlphaF;
  }
  else
  {
    myIsAlpha = thePixelFormat == GL_ALPHA;
  }

  myHasMipmaps             = toCreateMipMaps;
  myTextFormat             = thePixelFormat;
#if !defined(GL_ES_VERSION_2_0)
  const GLint anIntFormat  = theTextFormat;
#else
  // ES 2.0 does not support sized formats and format conversions - them detected from data type
  const GLint anIntFormat  = theCtx->IsGlGreaterEqual (3, 0) ? theTextFormat : thePixelFormat;
#endif

  if (theDataType == GL_FLOAT && !theCtx->arbTexFloat)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Error: floating-point textures are not supported by hardware.");
    Release (theCtx.operator->());
    return false;
  }

  const GLsizei aMaxSize = theCtx->MaxTextureSize();
  if (theSizeX > aMaxSize
   || theSizeY > aMaxSize)
  {
    TCollection_ExtendedString aWarnMessage = TCollection_ExtendedString ("Error: Texture dimension - ")
      + theSizeX + "x" + theSizeY + " exceeds hardware limits (" + aMaxSize + "x" + aMaxSize + ")";

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
    const GLsizei aWidthP2  = OpenGl_Context::GetPowerOfTwo (theSizeX, aMaxSize);
    const GLsizei aHeightP2 = OpenGl_Context::GetPowerOfTwo (theSizeY, aMaxSize);

    if (theSizeX != aWidthP2
     || (theType != Graphic3d_TOT_1D && theSizeY != aHeightP2))
    {
      TCollection_ExtendedString aWarnMessage =
        TCollection_ExtendedString ("Error: NPOT Textures (") + theSizeX + "x" + theSizeY + ") are not supported by hardware.";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);

      Release (theCtx.operator->());
      return false;
    }
  }
#else
  else if (!theCtx->IsGlGreaterEqual (3, 0) && theType == Graphic3d_TOT_2D_MIPMAP)
  {
    // Mipmap NPOT textures are not supported by OpenGL ES 2.0.
    const GLsizei aWidthP2  = OpenGl_Context::GetPowerOfTwo (theSizeX, aMaxSize);
    const GLsizei aHeightP2 = OpenGl_Context::GetPowerOfTwo (theSizeY, aMaxSize);

    if (theSizeX != aWidthP2
     || theSizeY != aHeightP2)
    {
      TCollection_ExtendedString aWarnMessage =
        TCollection_ExtendedString ("Error: Mipmap NPOT Textures (") + theSizeX + "x" + theSizeY + ") are not supported by OpenGL ES 2.0";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);

      Release (theCtx.operator->());
      return false;
    }
  }
#endif

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

  myTarget = aTarget;
  switch (theType)
  {
    case Graphic3d_TOT_1D:
    {
    #if !defined(GL_ES_VERSION_2_0)
      Bind (theCtx);
      applyDefaultSamplerParams (theCtx);
      if (toPatchExisting)
      {
        glTexSubImage1D (GL_TEXTURE_1D, 0, 0,
                         theSizeX, thePixelFormat, theDataType, aDataPtr);
        Unbind (theCtx);
        return true;
      }

      // use proxy to check texture could be created or not
      glTexImage1D (GL_PROXY_TEXTURE_1D, 0, anIntFormat,
                    theSizeX, 0,
                    thePixelFormat, theDataType, NULL);
      glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 0, GL_TEXTURE_WIDTH, &aTestWidth);
      if (aTestWidth == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      glTexImage1D (GL_TEXTURE_1D, 0, anIntFormat,
                    theSizeX, 0,
                    thePixelFormat, theDataType, aDataPtr);
      if (glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      mySizeX = theSizeX;
      mySizeY = 1;

      Unbind (theCtx);
      return true;
    #else
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           "Error: 1D textures are not supported by hardware.");
      Release (theCtx.operator->());
      return false;
    #endif
    }
    case Graphic3d_TOT_2D:
    {
      Bind (theCtx);
      applyDefaultSamplerParams (theCtx);
      if (toPatchExisting)
      {
        glTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         theSizeX, theSizeY,
                         thePixelFormat, theDataType, aDataPtr);
        Unbind (theCtx);
        return true;
      }

    #if !defined(GL_ES_VERSION_2_0)
      // use proxy to check texture could be created or not
      glTexImage2D (GL_PROXY_TEXTURE_2D, 0, anIntFormat,
                    theSizeX, theSizeY, 0,
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
                    theSizeX, theSizeY, 0,
                    thePixelFormat, theDataType, aDataPtr);
      const GLenum anErr = glGetError();
      if (anErr != GL_NO_ERROR)
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             TCollection_AsciiString ("Error: 2D texture ") + theSizeX + "x" + theSizeY
                                                   + " IF: " + int(anIntFormat) + " PF: " + int(thePixelFormat) + " DT: " + int(theDataType)
                                                   + " can not be created with error " + int(anErr) + ".");
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      mySizeX = theSizeX;
      mySizeY = theSizeY;

      Unbind (theCtx);
      return true;
    }
    case Graphic3d_TOT_2D_MIPMAP:
    {
      Bind (theCtx);
      applyDefaultSamplerParams (theCtx);
      if (toPatchExisting)
      {
        glTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         theSizeX, theSizeY,
                         thePixelFormat, theDataType, aDataPtr);
        if (theCtx->arbFBO != NULL)
        {
          // generate mipmaps
          theCtx->arbFBO->glGenerateMipmap (GL_TEXTURE_2D);
          if (glGetError() != GL_NO_ERROR)
          {
            Unbind (theCtx);
            Release (theCtx.operator->());
            return false;
          }
        }

        Unbind (theCtx);
        return true;
      }

    #if !defined(GL_ES_VERSION_2_0)
      // use proxy to check texture could be created or not
      glTexImage2D (GL_PROXY_TEXTURE_2D, 0, anIntFormat,
                    theSizeX, theSizeY, 0,
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
                    theSizeX, theSizeY, 0,
                    thePixelFormat, theDataType, theImage->Data());
      const GLenum aTexImgErr = glGetError();
      if (aTexImgErr != GL_NO_ERROR)
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             TCollection_AsciiString ("Error: 2D texture ") + theSizeX + "x" + theSizeY
                                                    + " IF: " + int(anIntFormat) + " PF: " + int(thePixelFormat) + " DT: " + int(theDataType)
                                                    + " can not be created with error " + int(aTexImgErr) + ".");
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      mySizeX = theSizeX;
      mySizeY = theSizeY;

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

        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);

        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      Unbind (theCtx);
      return true;
    }
  }

  Release (theCtx.operator->());
  return false;
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
// function : Init2DMultisample
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init2DMultisample (const Handle(OpenGl_Context)& theCtx,
                                        const GLsizei                 theNbSamples,
                                        const GLint                   theTextFormat,
                                        const GLsizei                 theSizeX,
                                        const GLsizei                 theSizeY)
{
  if (!Create (theCtx)
    || theNbSamples > theCtx->MaxMsaaSamples()
    || theNbSamples < 1)
  {
    return false;
  }

  const GLsizei aNbSamples = OpenGl_Context::GetPowerOfTwo (theNbSamples, theCtx->MaxMsaaSamples());
  myTarget = GL_TEXTURE_2D_MULTISAMPLE;
  if(theSizeX > theCtx->MaxTextureSize()
  || theSizeY > theCtx->MaxTextureSize())
  {
    return false;
  }

  Bind (theCtx);
  //myTextFormat = theTextFormat;
#if !defined(GL_ES_VERSION_2_0)
  if (theCtx->Functions()->glTexStorage2DMultisample != NULL)
  {
    theCtx->Functions()->glTexStorage2DMultisample (myTarget, aNbSamples, theTextFormat, theSizeX, theSizeY, GL_FALSE);
  }
  else
  {
    theCtx->Functions()->glTexImage2DMultisample   (myTarget, aNbSamples, theTextFormat, theSizeX, theSizeY, GL_FALSE);
  }
#else
  theCtx->Functions()  ->glTexStorage2DMultisample (myTarget, aNbSamples, theTextFormat, theSizeX, theSizeY, GL_FALSE);
#endif
  if (theCtx->core11fwd->glGetError() != GL_NO_ERROR)
  {
    Unbind (theCtx);
    return false;
  }

  mySizeX = theSizeX;
  mySizeY = theSizeY;

  Unbind (theCtx);
  return true;
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

  Bind (theCtx);
  applyDefaultSamplerParams (theCtx);

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
  (void )theSizeX;
  (void )theSizeY;
  (void )theFormat;
  return false;
#endif
}

// =======================================================================
// function : Init3D
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init3D (const Handle(OpenGl_Context)& theCtx,
                             const GLint                   theTextFormat,
                             const GLenum                  thePixelFormat,
                             const GLenum                  theDataType,
                             const Standard_Integer        theSizeX,
                             const Standard_Integer        theSizeY,
                             const Standard_Integer        theSizeZ,
                             const void*                   thePixels)
{
  if (theCtx->Functions()->glTexImage3D == NULL)
  {
    TCollection_ExtendedString aMsg ("Error: three-dimensional textures are not supported by hardware.");

    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                         GL_DEBUG_TYPE_ERROR,
                         0,
                         GL_DEBUG_SEVERITY_HIGH,
                         aMsg);

    return false;
  }

  if (!Create(theCtx))
  {
    return false;
  }

  myTarget = GL_TEXTURE_3D;

  const GLsizei aSizeX = Min (theCtx->MaxTextureSize(), theSizeX);
  const GLsizei aSizeY = Min (theCtx->MaxTextureSize(), theSizeY);
  const GLsizei aSizeZ = Min (theCtx->MaxTextureSize(), theSizeZ);

  Bind (theCtx);

  if (theDataType == GL_FLOAT && !theCtx->arbTexFloat)
  {
    TCollection_ExtendedString aMsg ("Error: floating-point textures are not supported by hardware.");

    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                         GL_DEBUG_TYPE_ERROR,
                         0,
                         GL_DEBUG_SEVERITY_HIGH,
                         aMsg);

    Release (theCtx.operator->());
    Unbind (theCtx);
    return false;
  }

  const GLint anIntFormat = theTextFormat;

#if !defined (GL_ES_VERSION_2_0)
  theCtx->core15fwd->glTexImage3D (GL_PROXY_TEXTURE_3D,
                                   0,
                                   anIntFormat,
                                   aSizeX,
                                   aSizeY,
                                   aSizeZ,
                                   0,
                                   thePixelFormat,
                                   theDataType,
                                   NULL);

  GLint aTestSizeX = 0;
  GLint aTestSizeY = 0;
  GLint aTestSizeZ = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &aTestSizeX);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &aTestSizeY);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &aTestSizeZ);

  if (aTestSizeX == 0 || aTestSizeY == 0 || aTestSizeZ == 0)
  {
    Unbind (theCtx);
    Release (theCtx.operator->());
    return false;
  }
#endif

  applyDefaultSamplerParams (theCtx);
  theCtx->Functions()->glTexImage3D (myTarget,
                                     0,
                                     anIntFormat,
                                     aSizeX,
                                     aSizeY,
                                     aSizeZ,
                                     0,
                                     thePixelFormat,
                                     theDataType,
                                     thePixels);

  if (glGetError() != GL_NO_ERROR)
  {
    Unbind (theCtx);
    Release (theCtx.operator->());
    return false;
  }

  mySizeX = aSizeX;
  mySizeY = aSizeY;
  mySizeZ = aSizeZ;

  Unbind (theCtx);
  return true;
}
