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
#include <OpenGl_GlCore45.hxx>
#include <OpenGl_Sampler.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Standard_Assert.hxx>
#include <Image_CompressedPixMap.hxx>
#include <Image_PixMap.hxx>
#include <Image_SupportedFormats.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Texture, OpenGl_NamedResource)

namespace
{

//! Simple class to reset unpack alignment settings
struct OpenGl_UnpackAlignmentSentry
{
  //! Reset unpack alignment settings to safe values
  static void Reset (const OpenGl_Context& theCtx)
  {
    theCtx.core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    if (theCtx.hasUnpackRowLength)
    {
      theCtx.core11fwd->glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
    }
  }

  OpenGl_UnpackAlignmentSentry (const Handle(OpenGl_Context)& theCtx)
  : myCtx (theCtx.get()) {}

  ~OpenGl_UnpackAlignmentSentry()
  {
    Reset (*myCtx);
  }

private:
  OpenGl_Context* myCtx;
};

//! Compute the upper mipmap level for complete mipmap set (e.g. till the 1x1 level).
static Standard_Integer computeUpperMipMapLevel (Standard_Integer theSize)
{
  for (Standard_Integer aMipIter = 0;; ++aMipIter, theSize /= 2)
  {
    if (theSize <= 1)
    {
      return aMipIter;
    }
  }
}

//! Compute size of the smallest defined mipmap level (for verbose messages).
static Graphic3d_Vec2i computeSmallestMipMapSize (const Graphic3d_Vec2i& theBaseSize, Standard_Integer theMaxLevel)
{
  Graphic3d_Vec2i aMipSizeXY = theBaseSize;
  for (Standard_Integer aMipIter = 0;; ++aMipIter)
  {
    if (aMipIter > theMaxLevel)
    {
      return aMipSizeXY;
    }

    aMipSizeXY /= 2;
    if (aMipSizeXY.x() == 0) { aMipSizeXY.x() = 1; }
    if (aMipSizeXY.y() == 0) { aMipSizeXY.y() = 1; }
  }
}

}

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
  myTextFormat (GL_RGBA),
  mySizedFormat(GL_RGBA8),
  myNbSamples  (1),
  myMaxMipLevel(0),
  myIsAlpha    (false),
  myIsTopDown  (true)
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
    theGlCtx->core11fwd->glDeleteTextures (1, &myTextureId);
  }
  myTextureId = NO_TEXTURE;
  mySize.SetValues (0, 0, 0);
}

// =======================================================================
// function : applyDefaultSamplerParams
// purpose  :
// =======================================================================
void OpenGl_Texture::applyDefaultSamplerParams (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Sampler::applySamplerParams (theCtx, mySampler->Parameters(), NULL, myTarget, myMaxMipLevel);
  if (mySampler->IsValid() && !mySampler->IsImmutable())
  {
    OpenGl_Sampler::applySamplerParams (theCtx, mySampler->Parameters(), mySampler.get(), myTarget, myMaxMipLevel);
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
  theCtx->core11fwd->glBindTexture (myTarget, myTextureId);
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
  theCtx->core11fwd->glBindTexture (myTarget, NO_TEXTURE);
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

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init (const Handle(OpenGl_Context)& theCtx,
                           const OpenGl_TextureFormat&   theFormat,
                           const Graphic3d_Vec3i&        theSizeXYZ,
                           const Graphic3d_TypeOfTexture theType,
                           const Image_PixMap*           theImage)
{
  if (theSizeXYZ.x() < 1
   || theSizeXYZ.y() < 1
   || theSizeXYZ.z() < 1)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: texture of 0 size cannot be created [") + myResourceId +"]");
    Release (theCtx.get());
    return false;
  }

  GLenum aTarget = GL_TEXTURE_2D;
  switch (theType)
  {
    case Graphic3d_TypeOfTexture_1D:
    {
      aTarget = theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES
              ? GL_TEXTURE_1D
              : GL_TEXTURE_2D;
      break;
    }
    case Graphic3d_TypeOfTexture_2D:
    case Graphic3d_TOT_2D_MIPMAP:
    {
      aTarget = GL_TEXTURE_2D;
      break;
    }
    case Graphic3d_TypeOfTexture_3D:
    {
      aTarget = GL_TEXTURE_3D;
      break;
    }
    case Graphic3d_TypeOfTexture_CUBEMAP:
    {
      aTarget = GL_TEXTURE_CUBE_MAP;
      break;
    }
  }
  const bool toPatchExisting = IsValid()
                            && myTextFormat == theFormat.PixelFormat()
                            && myTarget == aTarget
                            && mySize.x()  == theSizeXYZ.x()
                            && (mySize.y() == theSizeXYZ.y() || theType == Graphic3d_TypeOfTexture_1D)
                            && mySize.z()  == theSizeXYZ.z();
  if (!Create (theCtx))
  {
    Release (theCtx.get());
    return false;
  }

  if (theImage != NULL)
  {
    myIsAlpha = theImage->Format() == Image_Format_Alpha
             || theImage->Format() == Image_Format_AlphaF;
    myIsTopDown = theImage->IsTopDown();
  }
  else
  {
    myIsAlpha = theFormat.PixelFormat() == GL_ALPHA;
  }

  myMaxMipLevel = 0;
  myTextFormat  = theFormat.PixelFormat();
  mySizedFormat = theFormat.InternalFormat();
  myNbSamples   = 1;

  // ES 2.0 does not support sized formats and format conversions - them detected from data type
  const GLint anIntFormat  = (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES
                           || theCtx->IsGlGreaterEqual (3, 0))
                           ? theFormat.InternalFormat()
                           : theFormat.PixelFormat();

  if (theFormat.DataType() == GL_FLOAT
  && !theCtx->arbTexFloat)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: floating-point textures are not supported by hardware [") + myResourceId +"]");
    Release (theCtx.get());
    return false;
  }

  const Standard_Integer aMaxSize = theCtx->MaxTextureSize();
  if (theSizeXYZ.maxComp() > aMaxSize)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: Texture dimension - ") + theSizeXYZ.x() + "x" + theSizeXYZ.y()
                       + (theSizeXYZ.z() > 1 ? TCollection_AsciiString ("x") + theSizeXYZ.z() : TCollection_AsciiString())
                       + " exceeds hardware limits (" + aMaxSize + "x" + aMaxSize + ")"
                       + " [" + myResourceId +"]");
    Release (theCtx.get());
    return false;
  }
  else if (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGL
       && !theCtx->IsGlGreaterEqual (3, 0)
       && !theCtx->arbNPTW)
  {
    // Notice that formally general NPOT textures are required by OpenGL 2.0 specifications
    // however some hardware (NV30 - GeForce FX, RadeOn 9xxx and Xxxx) supports GLSL but not NPOT!
    // Trying to create NPOT textures on such hardware will not fail
    // but driver will fall back into software rendering,
    const Graphic3d_Vec2i aSizeP2 (OpenGl_Context::GetPowerOfTwo (theSizeXYZ.x(), aMaxSize),
                                   OpenGl_Context::GetPowerOfTwo (theSizeXYZ.y(), aMaxSize));
    if (theSizeXYZ.x() != aSizeP2.x()
     || (theType != Graphic3d_TypeOfTexture_1D && theSizeXYZ.y() != aSizeP2.y()))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Error: NPOT Textures (") + theSizeXYZ.x() + "x" + theSizeXYZ.y() + ")"
                           " are not supported by hardware [" + myResourceId +"]");
      Release (theCtx.get());
      return false;
    }
  }

  GLint aTestWidth = 0, aTestHeight = 0;
  GLvoid* aDataPtr = (theImage != NULL) ? (GLvoid* )theImage->Data() : NULL;

  // setup the alignment
  OpenGl_UnpackAlignmentSentry anUnpackSentry (theCtx);
  (void)anUnpackSentry; // avoid compiler warning

  if (aDataPtr != NULL)
  {
    const GLint anAligment = Min ((GLint )theImage->MaxRowAligmentBytes(), 8); // OpenGL supports alignment upto 8 bytes
    theCtx->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, anAligment);
    const GLint anExtraBytes = GLint(theImage->RowExtraBytes());
    const GLint aPixelsWidth = GLint(theImage->SizeRowBytes() / theImage->SizePixelBytes());
    if (theCtx->hasUnpackRowLength)
    {
      theCtx->core11fwd->glPixelStorei (GL_UNPACK_ROW_LENGTH, (anExtraBytes >= anAligment) ? aPixelsWidth : 0);
    }
    else if (anExtraBytes >= anAligment)
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Error: unsupported image stride within OpenGL ES 2.0 [") + myResourceId +"]");
      Release (theCtx.get());
      return false;
    }
  }

  myTarget = aTarget;
  switch (theType)
  {
    case Graphic3d_TypeOfTexture_1D:
    {
      if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             TCollection_AsciiString ( "Error: 1D textures are not supported by hardware [") + myResourceId +"]");
        Release (theCtx.get());
        return false;
      }

      Bind (theCtx);
      applyDefaultSamplerParams (theCtx);
      if (toPatchExisting)
      {
        theCtx->core11fwd->glTexSubImage1D (GL_TEXTURE_1D, 0, 0,
                                            theSizeXYZ.x(), theFormat.PixelFormat(), theFormat.DataType(), aDataPtr);
        break;
      }

      // use proxy to check texture could be created or not
      theCtx->core11fwd->glTexImage1D (GL_PROXY_TEXTURE_1D, 0, anIntFormat,
                                       theSizeXYZ.x(), 0,
                                       theFormat.PixelFormat(), theFormat.DataType(), NULL);
      theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 0, GL_TEXTURE_WIDTH, &aTestWidth);
      theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 0, GL_TEXTURE_INTERNAL_FORMAT, &mySizedFormat);
      if (aTestWidth == 0)
      {
        // no memory or broken input parameters
        Unbind (theCtx);
        Release (theCtx.operator->());
        return false;
      }

      theCtx->core11fwd->glTexImage1D (GL_TEXTURE_1D, 0, anIntFormat,
                                       theSizeXYZ.x(), 0,
                                       theFormat.PixelFormat(), theFormat.DataType(), aDataPtr);
      if (theCtx->core11fwd->glGetError() != GL_NO_ERROR)
      {
        Unbind (theCtx);
        Release (theCtx.get());
        return false;
      }

      mySize.SetValues (theSizeXYZ.x(), 1, 1);
      break;
    }
    case Graphic3d_TypeOfTexture_2D:
    case Graphic3d_TOT_2D_MIPMAP:
    {
      Bind (theCtx);
      applyDefaultSamplerParams (theCtx);
      if (toPatchExisting)
      {
        theCtx->core11fwd->glTexSubImage2D (GL_TEXTURE_2D, 0,
                                            0, 0,
                                            theSizeXYZ.x(), theSizeXYZ.y(),
                                            theFormat.PixelFormat(), theFormat.DataType(), aDataPtr);
        break;
      }

      if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGL)
      {
        // use proxy to check texture could be created or not
        theCtx->core11fwd->glTexImage2D (GL_PROXY_TEXTURE_2D, 0, anIntFormat,
                                         theSizeXYZ.x(), theSizeXYZ.y(), 0,
                                         theFormat.PixelFormat(), theFormat.DataType(), NULL);
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &aTestWidth);
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &aTestHeight);
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &mySizedFormat);
        if (aTestWidth == 0 || aTestHeight == 0)
        {
          // no memory or broken input parameters
          Unbind (theCtx);
          Release (theCtx.get());
          return false;
        }
      }

      theCtx->core11fwd->glTexImage2D (GL_TEXTURE_2D, 0, anIntFormat,
                                       theSizeXYZ.x(), theSizeXYZ.y(), 0,
                                       theFormat.PixelFormat(), theFormat.DataType(), aDataPtr);
      GLenum anErr = theCtx->core11fwd->glGetError();
      if (anErr != GL_NO_ERROR)
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             TCollection_AsciiString ("Error: 2D texture ") + theSizeXYZ.x() + "x" + theSizeXYZ.y()
                                                   + " IF: " + OpenGl_TextureFormat::FormatFormat (anIntFormat)
                                                   + " PF: " + OpenGl_TextureFormat::FormatFormat (theFormat.PixelFormat())
                                                   + " DT: " + OpenGl_TextureFormat::FormatDataType (theFormat.DataType())
                                                   + " can not be created with error " + OpenGl_Context::FormatGlError (anErr)
                                                   + " [" + myResourceId +"]");
        Unbind (theCtx);
        Release (theCtx.get());
        return false;
      }

      mySize.SetValues (theSizeXYZ.xy(), 1);
      break;
    }
    case Graphic3d_TypeOfTexture_3D:
    {
      if (theCtx->Functions()->glTexImage3D == nullptr)
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             "Error: three-dimensional textures are not supported by hardware.");
        Unbind (theCtx);
        Release (theCtx.get());
        return false;
      }

      Bind (theCtx);
      applyDefaultSamplerParams (theCtx);
      if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGL)
      {
        theCtx->Functions()->glTexImage3D (GL_PROXY_TEXTURE_3D, 0, anIntFormat,
                                           theSizeXYZ.x(), theSizeXYZ.y(), theSizeXYZ.z(), 0,
                                           theFormat.PixelFormat(), theFormat.DataType(),  nullptr);

        NCollection_Vec3<GLint> aTestSizeXYZ;
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH,  &aTestSizeXYZ.x());
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &aTestSizeXYZ.y());
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH,  &aTestSizeXYZ.z());
        theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_INTERNAL_FORMAT, &mySizedFormat);
        if (aTestSizeXYZ.x() == 0 || aTestSizeXYZ.y() == 0 || aTestSizeXYZ.z() == 0)
        {
          Unbind (theCtx);
          Release (theCtx.get());
          return false;
        }
      }

      theCtx->Functions()->glTexImage3D (GL_TEXTURE_3D, 0, anIntFormat,
                                         theSizeXYZ.x(), theSizeXYZ.y(), theSizeXYZ.z(), 0,
                                         theFormat.PixelFormat(), theFormat.DataType(), aDataPtr);
      GLenum anErr = theCtx->core11fwd->glGetError();
      if (anErr != GL_NO_ERROR)
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             TCollection_AsciiString ("Error: 3D texture ") + theSizeXYZ.x() + "x" + theSizeXYZ.y() + "x" + theSizeXYZ.z()
                                                   + " IF: " + OpenGl_TextureFormat::FormatFormat (anIntFormat)
                                                   + " PF: " + OpenGl_TextureFormat::FormatFormat (theFormat.PixelFormat())
                                                   + " DT: " + OpenGl_TextureFormat::FormatDataType (theFormat.DataType())
                                                   + " can not be created with error " + OpenGl_Context::FormatGlError (anErr)
                                                   + " [" + myResourceId +"]");
        Unbind (theCtx);
        Release (theCtx.get());
        return false;
      }

      mySize = theSizeXYZ;
      break;
    }
    case Graphic3d_TypeOfTexture_CUBEMAP:
    {
      Unbind (theCtx);
      Release (theCtx.get());
      return false;
    }
  }

  Unbind (theCtx);
  return true;
}

// =======================================================================
// function : GenerateMipmaps
// purpose  :
// =======================================================================
bool OpenGl_Texture::GenerateMipmaps (const Handle(OpenGl_Context)& theCtx)
{
  if (theCtx->arbFBO == nullptr
  || !IsValid())
  {
    return false;
  }

  myMaxMipLevel = computeUpperMipMapLevel (mySize.maxComp());

  const Standard_Integer aMaxSize = theCtx->MaxTextureSize();
  if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
  && !theCtx->IsGlGreaterEqual (3, 0))
  {
    // Mipmap NPOT textures are not supported by OpenGL ES 2.0.
    const Graphic3d_Vec2i aSizeP2 (OpenGl_Context::GetPowerOfTwo (mySize.x(), aMaxSize),
                                   OpenGl_Context::GetPowerOfTwo (mySize.y(), aMaxSize));
    if (mySize.xy() != aSizeP2)
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Warning: Mipmap NPOT Textures (") + mySize.x() + "x" + mySize.y() + ")"
                           " are not supported by OpenGL ES 2.0 [" + myResourceId +"]");
      myMaxMipLevel = 0;
    }
  }

  if (myMaxMipLevel <= 0)
  {
    return false;
  }

  //glHint (GL_GENERATE_MIPMAP_HINT, GL_NICEST);
  Bind (theCtx);
  if (theCtx->HasTextureBaseLevel()
  && !mySampler->isValidSampler())
  {
    const Standard_Integer aMaxLevel = Min (myMaxMipLevel, mySampler->Parameters()->MaxLevel());
    mySampler->SetParameter (theCtx, myTarget, GL_TEXTURE_MAX_LEVEL, aMaxLevel);
  }
  theCtx->arbFBO->glGenerateMipmap (myTarget);
  GLenum anErr = theCtx->core11fwd->glGetError();
  if (anErr != GL_NO_ERROR)
  {
    myMaxMipLevel = 0;
    if (theCtx->HasTextureBaseLevel()
    && !mySampler->isValidSampler())
    {
      mySampler->SetParameter (theCtx, myTarget, GL_TEXTURE_MAX_LEVEL, 0);
    }

    if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
      && (mySizedFormat == GL_RGB8
       || mySizedFormat == GL_SRGB8))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                            TCollection_AsciiString ("Warning: generating mipmaps requires color-renderable format, while giving ")
                            + OpenGl_TextureFormat::FormatFormat (mySizedFormat) + " [" + myResourceId +"]");
    }
    else
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                            TCollection_AsciiString ("Warning: generating mipmaps has failed [") + myResourceId +"]");
    }
  }

  applyDefaultSamplerParams (theCtx);
  Unbind (theCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init (const Handle(OpenGl_Context)& theCtx,
                           const Image_PixMap&           theImage,
                           const Graphic3d_TypeOfTexture theType,
                           const Standard_Boolean        theIsColorMap)
{
  if (theImage.IsEmpty())
  {
    Release (theCtx.get());
    return false;
  }

  const OpenGl_TextureFormat aFormat = OpenGl_TextureFormat::FindFormat (theCtx, theImage.Format(), theIsColorMap);
  if (!aFormat.IsValid())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: No suitable texture format for ") + Image_PixMap::ImageFormatToString (theImage.Format()) + " image format"
                         + " [" + myResourceId +"]");
    Release (theCtx.get());
    return false;
  }

  return Init (theCtx, aFormat, Graphic3d_Vec3i (theImage.SizeXYZ()), theType, &theImage);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init (const Handle(OpenGl_Context)& theCtx,
                           const Handle(Graphic3d_TextureRoot)& theTextureMap)
{
  if (theTextureMap.IsNull())
  {
    return false;
  }

  switch (theTextureMap->Type())
  {
    case Graphic3d_TypeOfTexture_CUBEMAP:
    {
      return InitCubeMap (theCtx, Handle(Graphic3d_CubeMap)::DownCast(theTextureMap),
                          0, Image_Format_RGB, false, theTextureMap->IsColorMap());
    }
    default:
    {
      if (theCtx->SupportedTextureFormats()->HasCompressed()
      && !theCtx->caps->compressedTexturesDisable)
      {
        if (Handle(Image_CompressedPixMap) aCompressed = theTextureMap->GetCompressedImage (theCtx->SupportedTextureFormats()))
        {
          return InitCompressed (theCtx, *aCompressed, theTextureMap->IsColorMap());
        }
      }

      Handle(Image_PixMap) anImage = theTextureMap->GetImage (theCtx->SupportedTextureFormats());
      if (anImage.IsNull())
      {
        return false;
      }
      if (!Init (theCtx, *anImage, theTextureMap->Type(), theTextureMap->IsColorMap()))
      {
        return false;
      }
      if (theTextureMap->HasMipmaps())
      {
        GenerateMipmaps (theCtx);
      }
      return true;
    }
  }
}

// =======================================================================
// function : InitCompressed
// purpose  :
// =======================================================================
bool OpenGl_Texture::InitCompressed (const Handle(OpenGl_Context)& theCtx,
                                     const Image_CompressedPixMap& theImage,
                                     const Standard_Boolean        theIsColorMap)
{
  if (theImage.SizeX() < 1
   || theImage.SizeY() < 1
   || theImage.FaceData().IsNull())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Error: texture of 0 size cannot be created.");
    Release (theCtx.get());
    return false;
  }
  if (theImage.SizeX() > theCtx->MaxTextureSize()
   || theImage.SizeY() > theCtx->MaxTextureSize())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: Texture dimension - ") + theImage.SizeX() + "x" + theImage.SizeY()
                       + " exceeds hardware limits (" + theCtx->MaxTextureSize() + "x" + theCtx->MaxTextureSize() + ")");
    Release (theCtx.get());
    return false;
  }

  const OpenGl_TextureFormat aFormat = OpenGl_TextureFormat::FindCompressedFormat (theCtx, theImage.CompressedFormat(), theIsColorMap);
  if (!aFormat.IsValid())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: No suitable texture format for ") + Image_PixMap::ImageFormatToString (theImage.CompressedFormat()) + " image format "
                         + " [" + myResourceId +"]");
    Release (theCtx.get());
    return false;
  }

  if (!Create (theCtx))
  {
    return false;
  }

  myTarget = GL_TEXTURE_2D;
  myNbSamples = 1;
  myTextFormat  = aFormat.Format();
  mySizedFormat = aFormat.Internal();
  myIsTopDown = theImage.IsTopDown();
  mySize.SetValues (theImage.SizeX(), theImage.SizeY(), 1);
  myMaxMipLevel = Max (theImage.MipMaps().Size() - 1, 0);
  if (myMaxMipLevel > 0
  && !theImage.IsCompleteMipMapSet())
  {
    const Graphic3d_Vec2i aMipSize = computeSmallestMipMapSize (mySize.xy(), myMaxMipLevel);
    if (!theCtx->HasTextureBaseLevel())
    {
      myMaxMipLevel = 0;
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_MEDIUM,
                           TCollection_AsciiString ("Warning: compressed 2D texture ") + myResourceId + " " + mySize.x() + "x" + mySize.y()
                           + " has smallest mipmap " + aMipSize.x() + "x" + aMipSize.y() + "; mipmaps will be ignored");
    }
    else
    {
      Message::SendTrace (TCollection_AsciiString ("Warning: compressed 2D texture ") + myResourceId + " " + mySize.x() + "x" + mySize.y()
                          + " has smallest mipmap " + aMipSize.x() + "x" + aMipSize.y());
    }
  }

  Bind (theCtx);
  applyDefaultSamplerParams (theCtx);

  // setup the alignment
  OpenGl_UnpackAlignmentSentry::Reset (*theCtx);

  Graphic3d_Vec2i aMipSizeXY (theImage.SizeX(), theImage.SizeY());
  const Standard_Byte* aData = theImage.FaceData()->Data();
  for (Standard_Integer aMipIter = 0; aMipIter <= myMaxMipLevel; ++aMipIter)
  {
    const Standard_Integer aMipLength = theImage.MipMaps().Value (aMipIter);
    theCtx->Functions()->glCompressedTexImage2D (GL_TEXTURE_2D, aMipIter, mySizedFormat, aMipSizeXY.x(), aMipSizeXY.y(), 0, aMipLength, aData);
    const GLenum aTexImgErr = theCtx->core11fwd->glGetError();
    if (aTexImgErr != GL_NO_ERROR)
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Error: 2D compressed texture ") + aMipSizeXY.x() + "x" + aMipSizeXY.y()
                           + " IF: " + OpenGl_TextureFormat::FormatFormat (aFormat.Internal())
                           + " PF: " + OpenGl_TextureFormat::FormatFormat (aFormat.PixelFormat())
                           + " DT: " + OpenGl_TextureFormat::FormatDataType (aFormat.DataType())
                           + " can not be created with error " + OpenGl_Context::FormatGlError (aTexImgErr) + ".");
      Unbind (theCtx);
      Release (theCtx.get());
      return false;
    }

    aData += aMipLength;
    aMipSizeXY /= 2;
    if (aMipSizeXY.x() == 0) { aMipSizeXY.x() = 1; }
    if (aMipSizeXY.y() == 0) { aMipSizeXY.y() = 1; }
  }

  Unbind (theCtx);
  return true;
}

// =======================================================================
// function : Init2DMultisample
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init2DMultisample (const Handle(OpenGl_Context)& theCtx,
                                        const Standard_Integer theNbSamples,
                                        const Standard_Integer theTextFormat,
                                        const Standard_Integer theSizeX,
                                        const Standard_Integer theSizeY)
{
  if (!Create (theCtx)
   ||  theNbSamples > theCtx->MaxMsaaSamples()
   ||  theNbSamples < 1)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: MSAA texture ") + theSizeX + "x" + theSizeY + "@" + myNbSamples
                         + " exceeds samples limit: " + theCtx->MaxMsaaSamples() + ".");
    return false;
  }

  myNbSamples = OpenGl_Context::GetPowerOfTwo (theNbSamples, theCtx->MaxMsaaSamples());
  myTarget = GL_TEXTURE_2D_MULTISAMPLE;
  myMaxMipLevel = 0;
  if(theSizeX > theCtx->MaxTextureSize()
  || theSizeY > theCtx->MaxTextureSize())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: MSAA texture ") + theSizeX + "x" + theSizeY + "@" + myNbSamples
                         + " exceeds size limit: " + theCtx->MaxTextureSize() + "x" + theCtx->MaxTextureSize() + ".");
    return false;
  }

  Bind (theCtx);
  //myTextFormat = theTextFormat;
  mySizedFormat = theTextFormat;
  if (theCtx->HasTextureMultisampling()
   && theCtx->Functions()->glTexStorage2DMultisample != NULL)   // OpenGL 4.3
  {
    theCtx->Functions()->glTexStorage2DMultisample (myTarget, myNbSamples, theTextFormat, theSizeX, theSizeY, GL_FALSE);
  }
  else if (theCtx->HasTextureMultisampling()
        && theCtx->Functions()->glTexImage2DMultisample != NULL) // OpenGL 3.2
  {
    theCtx->Functions()->glTexImage2DMultisample   (myTarget, myNbSamples, theTextFormat, theSizeX, theSizeY, GL_FALSE);
  }
  else
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Error: MSAA textures are not supported by hardware.");
    Unbind (theCtx);
    return false;
  }

  const GLenum aTexImgErr = theCtx->core11fwd->glGetError();
  if (aTexImgErr != GL_NO_ERROR)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: MSAA texture ") + theSizeX + "x" + theSizeY + "@" + myNbSamples
                         + " IF: " + OpenGl_TextureFormat::FormatFormat (theTextFormat)
                         + " cannot be created with error " + OpenGl_Context::FormatGlError (aTexImgErr) + ".");
    Unbind (theCtx);
    return false;
  }

  mySize.SetValues (theSizeX, theSizeY, 1);

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
  if (!theCtx->IsGlGreaterEqual (3, 0)
   || !Create (theCtx)
   ||  theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    return false;
  }

  myTarget = GL_TEXTURE_RECTANGLE;
  myNbSamples = 1;
  myMaxMipLevel = 0;

  const GLsizei aSizeX = Min (theCtx->MaxTextureSize(), theSizeX);
  const GLsizei aSizeY = Min (theCtx->MaxTextureSize(), theSizeY);

  Bind (theCtx);
  applyDefaultSamplerParams (theCtx);

  myTextFormat  = theFormat.Format();
  mySizedFormat = theFormat.Internal();

  // setup the alignment
  OpenGl_UnpackAlignmentSentry::Reset (*theCtx);

  theCtx->core11fwd->glTexImage2D (GL_PROXY_TEXTURE_RECTANGLE, 0, mySizedFormat,
                                   aSizeX, aSizeY, 0,
                                   myTextFormat, GL_FLOAT, NULL);

  GLint aTestSizeX = 0, aTestSizeY = 0;
  theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_RECTANGLE, 0, GL_TEXTURE_WIDTH,  &aTestSizeX);
  theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_RECTANGLE, 0, GL_TEXTURE_HEIGHT, &aTestSizeY);
  theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_RECTANGLE, 0, GL_TEXTURE_INTERNAL_FORMAT, &mySizedFormat);
  if (aTestSizeX == 0 || aTestSizeY == 0)
  {
    Unbind (theCtx);
    return false;
  }

  theCtx->core11fwd->glTexImage2D (myTarget, 0, mySizedFormat,
                                   aSizeX, aSizeY, 0,
                                   myTextFormat, GL_FLOAT, NULL);
  if (theCtx->core11fwd->glGetError() != GL_NO_ERROR)
  {
    Unbind (theCtx);
    return false;
  }

  mySize.SetValues (aSizeX, aSizeY, 1);
  Unbind (theCtx);
  return true;
}

// =======================================================================
// function : Init3D
// purpose  :
// =======================================================================
bool OpenGl_Texture::Init3D (const Handle(OpenGl_Context)& theCtx,
                             const OpenGl_TextureFormat&   theFormat,
                             const Graphic3d_Vec3i&        theSizeXYZ,
                             const void*                   thePixels)
{
  if (theCtx->Functions()->glTexImage3D == NULL)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Error: three-dimensional textures are not supported by hardware.");
    return false;
  }

  if (!Create(theCtx))
  {
    return false;
  }

  myTarget = GL_TEXTURE_3D;
  myNbSamples = 1;
  myMaxMipLevel = 0;

  const Graphic3d_Vec3i aSizeXYZ = theSizeXYZ.cwiseMin (Graphic3d_Vec3i (theCtx->MaxTextureSize()));
  if (aSizeXYZ != theSizeXYZ)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Error: 3D texture dimensions exceed hardware limits.");
    Release (theCtx.get());
    Unbind (theCtx);
    return false;
  }
  Bind (theCtx);

  if (theFormat.DataType() == GL_FLOAT
  && !theCtx->arbTexFloat)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Error: floating-point textures are not supported by hardware.");
    Release (theCtx.get());
    Unbind (theCtx);
    return false;
  }

  mySizedFormat = theFormat.InternalFormat();

  // setup the alignment
  OpenGl_UnpackAlignmentSentry::Reset (*theCtx);

  if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGL)
  {
    theCtx->Functions()->glTexImage3D (GL_PROXY_TEXTURE_3D, 0, mySizedFormat,
                                       aSizeXYZ.x(), aSizeXYZ.y(), aSizeXYZ.z(), 0,
                                       theFormat.PixelFormat(), theFormat.DataType(), NULL);

    NCollection_Vec3<GLint> aTestSizeXYZ;
    theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH,  &aTestSizeXYZ.x());
    theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &aTestSizeXYZ.y());
    theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH,  &aTestSizeXYZ.z());
    theCtx->core11fwd->glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_INTERNAL_FORMAT, &mySizedFormat);
    if (aTestSizeXYZ.x() == 0 || aTestSizeXYZ.y() == 0 || aTestSizeXYZ.z() == 0)
    {
      Unbind (theCtx);
      Release (theCtx.get());
      return false;
    }
  }

  applyDefaultSamplerParams (theCtx);
  theCtx->Functions()->glTexImage3D (myTarget, 0, mySizedFormat,
                                     aSizeXYZ.x(), aSizeXYZ.y(), aSizeXYZ.z(), 0,
                                     theFormat.PixelFormat(), theFormat.DataType(), thePixels);

  if (theCtx->core11fwd->glGetError() != GL_NO_ERROR)
  {
    Unbind (theCtx);
    Release (theCtx.get());
    return false;
  }

  mySize = aSizeXYZ;

  Unbind (theCtx);
  return true;
}

// =======================================================================
// function : InitCubeMap
// purpose  :
// =======================================================================
bool OpenGl_Texture::InitCubeMap (const Handle(OpenGl_Context)&    theCtx,
                                  const Handle(Graphic3d_CubeMap)& theCubeMap,
                                  Standard_Size    theSize,
                                  Image_Format     theFormat,
                                  Standard_Boolean theToGenMipmap,
                                  Standard_Boolean theIsColorMap)
{
  if (!Create (theCtx))
  {
    Release (theCtx.get());
    return false;
  }

  Handle(Image_PixMap) anImage;
  Handle(Image_CompressedPixMap) aCompImage;
  OpenGl_TextureFormat aFormat;
  myMaxMipLevel = 0;
  if (!theCubeMap.IsNull())
  {
    theCubeMap->Reset();
    if (theCtx->SupportedTextureFormats()->HasCompressed()
    && !theCtx->caps->compressedTexturesDisable)
    {
      aCompImage = theCubeMap->CompressedValue (theCtx->SupportedTextureFormats());
    }
    if (!aCompImage.IsNull())
    {
      aFormat = OpenGl_TextureFormat::FindCompressedFormat (theCtx, aCompImage->CompressedFormat(), theIsColorMap);
      if (aFormat.IsValid())
      {
        theToGenMipmap = false;
        theSize   = aCompImage->SizeX();
        theFormat = aCompImage->BaseFormat();
        myMaxMipLevel = Max (aCompImage->MipMaps().Size() - 1, 0);
        if (myMaxMipLevel > 0
        && !aCompImage->IsCompleteMipMapSet())
        {
          const Graphic3d_Vec2i aMipSize = computeSmallestMipMapSize (Graphic3d_Vec2i (aCompImage->SizeX(), aCompImage->SizeY()), myMaxMipLevel);
          if (!theCtx->HasTextureBaseLevel())
          {
            myMaxMipLevel = 0;
            theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_MEDIUM,
                                 TCollection_AsciiString ("Warning: Cubemap compressed texture ") + theCubeMap->GetId() + " " + aCompImage->SizeX() + "x" + aCompImage->SizeX()
                                 + " has smallest mipmap " + aMipSize.x() + "x" + aMipSize.y() + "; mipmaps will be ignored");
          }
          else
          {
            Message::SendTrace (TCollection_AsciiString ("Warning: Cubemap compressed texture ") + theCubeMap->GetId() + " " + aCompImage->SizeX() + "x" + aCompImage->SizeX()
                                + " has smallest mipmap " + aMipSize.x() + "x" + aMipSize.y());
          }
        }

        OpenGl_UnpackAlignmentSentry::Reset (*theCtx);
      }
      else
      {
        aCompImage.Nullify();
      }
    }

    if (!aFormat.IsValid())
    {
      anImage = theCubeMap->Reset().Value (theCtx->SupportedTextureFormats());
      if (anImage.IsNull())
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             "Unable to get the first side of cubemap");
        Release(theCtx.get());
        return false;
      }

      theSize   = anImage->SizeX();
      theFormat = anImage->Format();
      theToGenMipmap = theCubeMap->HasMipmaps();
    }

    myIsTopDown = theCubeMap->IsTopDown();
  }

  if (!aFormat.IsValid())
  {
    aFormat = OpenGl_TextureFormat::FindFormat (theCtx, theFormat, theIsColorMap);
  }
  if (!aFormat.IsValid())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Error: No suitable texture format for ") + Image_PixMap::ImageFormatToString (theFormat) + " image format"
                         + " [" + myResourceId +"]");
    Unbind(theCtx);
    Release(theCtx.get());
    return false;
  }

  if (theToGenMipmap
  &&  theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
  && !theCtx->IsGlGreaterEqual (3, 0)
  &&  (aFormat.PixelFormat() == GL_SRGB_EXT
    || aFormat.PixelFormat() == GL_SRGB_ALPHA_EXT))
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("Warning, GL_EXT_sRGB disallows generation of mipmaps - fallback using non-sRGB format")
                         + " [" + myResourceId +"]");
    aFormat.SetPixelFormat   (aFormat.PixelFormat() == GL_SRGB_EXT ? GL_RGB  : GL_RGBA);
    aFormat.SetInternalFormat(aFormat.PixelFormat() == GL_SRGB_EXT ? GL_RGB8 : GL_RGBA8);
  }

  myTarget = GL_TEXTURE_CUBE_MAP;
  myNbSamples = 1;
  mySize.SetValues ((GLsizei )theSize, (GLsizei )theSize, 1);
  myTextFormat  = aFormat.Format();
  mySizedFormat = aFormat.Internal();

  // ES 2.0 does not support sized formats and format conversions - them detected from data type
  const GLint anIntFormat = (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES
                          || theCtx->IsGlGreaterEqual (3, 0))
                          ? aFormat.InternalFormat()
                          : aFormat.PixelFormat();

  Bind (theCtx);
  applyDefaultSamplerParams (theCtx);

  for (Standard_Integer i = 0; i < 6; ++i)
  {
    const Standard_Byte* aData = NULL;

    if (!theCubeMap.IsNull())
    {
      if (i != 0)
      {
        if (!aCompImage.IsNull())
        {
          aCompImage = theCubeMap->CompressedValue (theCtx->SupportedTextureFormats());
        }
        else
        {
          anImage = theCubeMap->Value (theCtx->SupportedTextureFormats());
        }
      }
      if (!aCompImage.IsNull())
      {
        Graphic3d_Vec2i aMipSizeXY = mySize.xy();
        aData = aCompImage->FaceData()->Data();
        for (Standard_Integer aMipIter = 0; aMipIter <= myMaxMipLevel; ++aMipIter)
        {
          const Standard_Integer aMipLength = aCompImage->MipMaps().Value (aMipIter);
          theCtx->Functions()->glCompressedTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, aMipIter, mySizedFormat, aMipSizeXY.x(), aMipSizeXY.y(), 0, aMipLength, aData);
          const GLenum aTexImgErr = theCtx->core11fwd->glGetError();
          if (aTexImgErr != GL_NO_ERROR)
          {
            theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                                 TCollection_AsciiString ("Error: cubemap compressed texture ") + aMipSizeXY.x() + "x" + aMipSizeXY.y()
                                 + " IF: " + OpenGl_TextureFormat::FormatFormat (aFormat.Internal())
                                 + " PF: " + OpenGl_TextureFormat::FormatFormat (aFormat.PixelFormat())
                                 + " DT: " + OpenGl_TextureFormat::FormatDataType (aFormat.DataType())
                                 + " can not be created with error " + OpenGl_Context::FormatGlError (aTexImgErr) + ".");
            Unbind (theCtx);
            Release (theCtx.get());
            return false;
          }

          aData += aMipLength;
          aMipSizeXY /= 2;
          if (aMipSizeXY.x() == 0) { aMipSizeXY.x() = 1; }
          if (aMipSizeXY.y() == 0) { aMipSizeXY.y() = 1; }
        }

        theCubeMap->Next();
        continue;
      }

      if (!anImage.IsNull())
      {
        const GLint anAligment = Min ((GLint)anImage->MaxRowAligmentBytes(), 8); // OpenGL supports alignment upto 8 bytes
        const GLint anExtraBytes = GLint(anImage->RowExtraBytes());
        const GLint aPixelsWidth = GLint(anImage->SizeRowBytes() / anImage->SizePixelBytes());
        const GLint aRowLength = (anExtraBytes >= anAligment) ? aPixelsWidth : 0;
        if (theCtx->hasUnpackRowLength)
        {
          theCtx->core11fwd->glPixelStorei (GL_UNPACK_ROW_LENGTH, aRowLength);
        }

        if (aRowLength > 0
        && !theCtx->hasUnpackRowLength)
        {
          Handle(Image_PixMap) aCopyImage = new Image_PixMap();
          aCopyImage->InitTrash (theFormat, theSize, theSize);
          const Standard_Size aRowBytesPacked = std::min (aCopyImage->SizeRowBytes(), anImage->SizeRowBytes());
          for (unsigned int y = 0; y < theSize; ++y)
          {
            memcpy (aCopyImage->ChangeRow (y), anImage->ChangeRow (y), aRowBytesPacked);
          }
          anImage = aCopyImage;
          const GLint anAligment2 = Min((GLint)anImage->MaxRowAligmentBytes(), 8); // OpenGL supports alignment upto 8 bytes
          theCtx->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, anAligment2);
        }
        else
        {
          theCtx->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, anAligment);
        }

        aData = anImage->Data();
      }
      else
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             TCollection_AsciiString() + "Unable to get [" + i + "] side of cubemap");
        Unbind (theCtx);
        Release (theCtx.get());
        return false;
      }
      theCubeMap->Next();
    }

    theCtx->core11fwd->glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                                     anIntFormat,
                                     GLsizei(theSize), GLsizei(theSize),
                                     0, aFormat.PixelFormat(), aFormat.DataType(),
                                     aData);

    OpenGl_UnpackAlignmentSentry::Reset (*theCtx);

    const GLenum anErr = theCtx->core11fwd->glGetError();
    if (anErr != GL_NO_ERROR)
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Error: cubemap side  ") + (int )theSize + "x" + (int )theSize
                           + " IF: " + OpenGl_TextureFormat::FormatFormat (anIntFormat)
                           + " PF: " + OpenGl_TextureFormat::FormatFormat (aFormat.PixelFormat())
                           + " DT: " + OpenGl_TextureFormat::FormatDataType (aFormat.DataType())
                           + " can not be created with error " + OpenGl_Context::FormatGlError (anErr) + ".");
      Unbind (theCtx);
      Release (theCtx.get());
      return false;
    }
  }

  if (theToGenMipmap && theCtx->arbFBO != NULL)
  {
    GenerateMipmaps (theCtx);
  }

  Unbind (theCtx.get());
  return true;
}

// =======================================================================
// function : PixelSizeOfPixelFormat
// purpose  :
// =======================================================================
Standard_Size OpenGl_Texture::PixelSizeOfPixelFormat (Standard_Integer theInternalFormat)
{
  switch(theInternalFormat)
  {
    // RED variations (GL_RED, OpenGL 3.0+)
    case GL_RED:
    case GL_R8:       return 1;
    case GL_R16:      return 2;
    case GL_R16F:     return 2;
    case GL_R32F:     return 4;
    // RGB variations
    case GL_RGB:      return 3;
    case GL_RGB8:     return 3;
    case GL_RGB16:    return 6;
    case GL_RGB16F:   return 6;
    case GL_RGB32F:   return 12;
    // RGBA variations
    case GL_RGBA:     return 4;
    case GL_RGBA8:    return 4;
    case GL_RGB10_A2: return 4;
    case GL_RGBA12:   return 6;
    case GL_RGBA16:   return 8;
    case GL_RGBA16F:  return 8;
    case GL_RGBA32F:  return 16;
    //
    case GL_BGRA_EXT:  return 4;
    // ALPHA variations (deprecated)
    case GL_ALPHA:
    case GL_ALPHA8:    return 1;
    case GL_ALPHA16:   return 2;
    case GL_LUMINANCE: return 1;
    case GL_LUMINANCE_ALPHA: return 2;
    // depth-stencil
    case GL_DEPTH24_STENCIL8:   return 4;
    case GL_DEPTH32F_STENCIL8:  return 8;
    case GL_DEPTH_COMPONENT16:  return 2;
    case GL_DEPTH_COMPONENT24:  return 3;
    case GL_DEPTH_COMPONENT32F: return 4;
    // compressed
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:  // DXT1 uses circa half a byte per pixel (64 bits per 4x4 block)
    case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: // DXT3/5 uses circa 1 byte per pixel (128 bits per 4x4 block)
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
      return 1;
  }
  return 1;
}

// =======================================================================
// function : EstimatedDataSize
// purpose  :
// =======================================================================
Standard_Size OpenGl_Texture::EstimatedDataSize() const
{
  if (!IsValid())
  {
    return 0;
  }

  Standard_Size aSize = PixelSizeOfPixelFormat (mySizedFormat) * mySize.x() * myNbSamples;
  if (mySize.y() != 0)
  {
    aSize *= Standard_Size(mySize.y());
  }
  if (mySize.z() != 0)
  {
    aSize *= Standard_Size(mySize.z());
  }
  if (myTarget == GL_TEXTURE_CUBE_MAP)
  {
    aSize *= 6; // cube sides
  }
  if (myMaxMipLevel > 0)
  {
    aSize = aSize + aSize / 3;
  }
  return aSize;
}

// =======================================================================
// function : ImageDump
// purpose  :
// =======================================================================
bool OpenGl_Texture::ImageDump (Image_PixMap& theImage,
                                const Handle(OpenGl_Context)& theCtx,
                                Graphic3d_TextureUnit theTexUnit,
                                Standard_Integer theLevel,
                                Standard_Integer theCubeSide) const
{
  const OpenGl_TextureFormat aFormat = OpenGl_TextureFormat::FindSizedFormat (theCtx, mySizedFormat);
  if (theCtx.IsNull()
  || !IsValid()
  ||  theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES // glGetTexImage() is unavailable in OpenGL ES
  ||  theLevel < 0
  || !aFormat.IsValid()
  ||  aFormat.ImageFormat() == Image_Format_UNKNOWN
  || (myTarget == GL_TEXTURE_CUBE_MAP
   && (theCubeSide < 0 || theCubeSide > 5)))
  {
    return false;
  }

  GLenum aTarget = myTarget;
  Graphic3d_Vec2i aSize = mySize.xy();
  if (myTarget == GL_TEXTURE_CUBE_MAP)
  {
    aTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + theCubeSide;
  }
  for (Standard_Integer aMipIter = 0; aMipIter < theLevel; ++aMipIter)
  {
    aSize /= 2;
    if (aSize.x() == 0) { aSize.x() = 1; }
    if (aSize.y() == 0) { aSize.y() = 1; }
  }
  if (!theImage.InitTrash (aFormat.ImageFormat(), aSize.x(), aSize.y()))
  {
    return false;
  }

  const GLint anAligment = Min (GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL
  theCtx->core11fwd->glPixelStorei (GL_PACK_ALIGNMENT, anAligment);
  if (theCtx->hasPackRowLength)
  {
    theCtx->core11fwd->glPixelStorei (GL_PACK_ROW_LENGTH, 0);
  }
  // glGetTextureImage() allows avoiding to binding texture id, but apparently requires clean FBO binding state...
  //if (theCtx->core45 != NULL) { theCtx->core45->glGetTextureImage (myTextureId, theLevel, aFormat.PixelFormat(), aFormat.DataType(), (GLsizei )theImage.SizeBytes(), theImage.ChangeData()); } else
  {
    Bind (theCtx, theTexUnit);
    theCtx->core11fwd->glGetTexImage (aTarget, theLevel, aFormat.PixelFormat(), aFormat.DataType(), theImage.ChangeData());
    Unbind (theCtx, theTexUnit);
  }
  if (theImage.Format() != aFormat.ImageFormat())
  {
    Image_PixMap::SwapRgbaBgra (theImage);
  }

  const bool hasErrors = theCtx->ResetErrors (true);
  theCtx->core11fwd->glPixelStorei (GL_PACK_ALIGNMENT, 1);
  return !hasErrors;
}
