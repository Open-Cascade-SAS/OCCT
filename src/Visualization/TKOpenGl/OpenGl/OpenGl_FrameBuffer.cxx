// Created by: Kirill GAVRILOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_FrameBuffer.hxx>

#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Texture.hxx>

#include <Standard_Assert.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_FrameBuffer, OpenGl_NamedResource)

namespace
{
//! Checks whether two format arrays are equal or not.
static bool operator==(const NCollection_Vector<int>& theFmt1,
                       const NCollection_Vector<int>& theFmt2)
{
  if (theFmt1.Length() != theFmt2.Length())
    return false;
  NCollection_Vector<int>::Iterator anIt1(theFmt1);
  NCollection_Vector<int>::Iterator anIt2(theFmt1);
  for (; anIt1.More(); anIt1.Next(), anIt2.Next())
  {
    if (anIt1.Value() != anIt2.Value())
      return false;
  }
  return true;
}

//! Return TRUE if GL_DEPTH_STENCIL_ATTACHMENT can be used.
static bool hasDepthStencilAttach(const occ::handle<OpenGl_Context>& theCtx)
{
#ifdef __EMSCRIPTEN__
  // supported since WebGL 2.0,
  // while WebGL 1.0 + GL_WEBGL_depth_texture needs GL_DEPTH_STENCIL_ATTACHMENT
  // and NOT separate GL_DEPTH_ATTACHMENT+GL_STENCIL_ATTACHMENT calls which is different to OpenGL
  // ES 2.0 + extension
  return theCtx->IsGlGreaterEqual(3, 0) || theCtx->extPDS;
#else
  // supported since OpenGL ES 3.0,
  // while OpenGL ES 2.0 + GL_EXT_packed_depth_stencil needs separate
  // GL_DEPTH_ATTACHMENT+GL_STENCIL_ATTACHMENT calls
  //
  // available on desktop since OpenGL 3.0
  // or OpenGL 2.0 + GL_ARB_framebuffer_object (GL_EXT_framebuffer_object is unsupported by OCCT)
  return theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES
         || theCtx->IsGlGreaterEqual(3, 0);
#endif
}
} // namespace

//=================================================================================================

OpenGl_FrameBuffer::OpenGl_FrameBuffer(const TCollection_AsciiString& theResourceId)
    : OpenGl_NamedResource(theResourceId),
      myInitVPSizeX(0),
      myInitVPSizeY(0),
      myVPSizeX(0),
      myVPSizeY(0),
      myNbSamples(0),
      myDepthFormat(GL_DEPTH24_STENCIL8),
      myGlFBufferId(NO_FRAMEBUFFER),
      myGlColorRBufferId(NO_RENDERBUFFER),
      myGlDepthRBufferId(NO_RENDERBUFFER),
      myIsOwnBuffer(false),
      myIsOwnColor(false),
      myIsOwnDepth(false),
      myDepthStencilTexture(new OpenGl_Texture(theResourceId + ":depth_stencil"))
{
  myColorFormats.Append(GL_RGBA8);
  myColorTextures.Append(new OpenGl_Texture(theResourceId + ":color"));
}

//=================================================================================================

OpenGl_FrameBuffer::~OpenGl_FrameBuffer()
{
  Release(NULL);
}

//=================================================================================================

int OpenGl_FrameBuffer::GetSizeX() const
{
  return !myColorTextures.IsEmpty() ? myColorTextures.First()->SizeX() : 0;
}

//=================================================================================================

int OpenGl_FrameBuffer::GetSizeY() const
{
  return !myColorTextures.IsEmpty() ? myColorTextures.First()->SizeY() : 0;
}

//=================================================================================================

bool OpenGl_FrameBuffer::InitWrapper(
  const occ::handle<OpenGl_Context>&                       theGlContext,
  const NCollection_Sequence<occ::handle<OpenGl_Texture>>& theColorTextures,
  const occ::handle<OpenGl_Texture>&                       theDepthTexture)
{
  Release(theGlContext.get());
  if (theGlContext->arbFBO == NULL)
  {
    return false;
  }

  myColorFormats.Clear();
  myColorTextures.Clear();
  for (NCollection_Sequence<occ::handle<OpenGl_Texture>>::Iterator aColorIter(theColorTextures);
       aColorIter.More();
       aColorIter.Next())
  {
    myColorTextures.Append(aColorIter.Value());
  }
  myDepthFormat         = 0;
  myDepthStencilTexture = theDepthTexture;
  myNbSamples           = theColorTextures.First()->NbSamples();

  myIsOwnColor  = false;
  myIsOwnDepth  = false;
  myIsOwnBuffer = true;

  myVPSizeX = theColorTextures.First()->SizeX();
  myVPSizeY = theColorTextures.First()->SizeY();

  theGlContext->arbFBO->glGenFramebuffers(1, &myGlFBufferId);
  theGlContext->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, myGlFBufferId);
  for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
  {
    const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
    if (aColorTexture->IsValid())
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + aColorBufferIdx,
                                                   aColorTexture->GetTarget(),
                                                   aColorTexture->TextureId(),
                                                   0);
    }
  }
  if (!myDepthStencilTexture.IsNull() && myDepthStencilTexture->IsValid())
  {
    if (hasDepthStencilAttach(theGlContext))
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_DEPTH_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_STENCIL_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
    }
  }
  if (theGlContext->arbFBO->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Release(theGlContext.get());
    return false;
  }

  UnbindBuffer(theGlContext);
  return true;
}

//=================================================================================================

bool OpenGl_FrameBuffer::Init(const occ::handle<OpenGl_Context>& theGlContext,
                              const NCollection_Vec2<int>&       theSize,
                              const int                          theColorFormat,
                              const int                          theDepthFormat,
                              const int                          theNbSamples)
{
  NCollection_Vector<int> aColorFormats;
  if (theColorFormat != 0)
  {
    aColorFormats.Append(theColorFormat);
  }
  return Init(theGlContext, theSize, aColorFormats, theDepthFormat, theNbSamples);
}

//=================================================================================================

bool OpenGl_FrameBuffer::Init(const occ::handle<OpenGl_Context>& theGlContext,
                              const NCollection_Vec2<int>&       theSize,
                              const NCollection_Vector<int>&     theColorFormats,
                              const occ::handle<OpenGl_Texture>& theDepthStencilTexture,
                              const int                          theNbSamples)
{
  myColorFormats = theColorFormats;

  OpenGl_TextureArray aTextures(myColorTextures);
  if (!myColorTextures.IsEmpty())
  {
    for (OpenGl_TextureArray::Iterator aTextureIt(myColorTextures); aTextureIt.More();
         aTextureIt.Next())
    {
      aTextureIt.Value()->Release(theGlContext.operator->());
    }
    myColorTextures.Clear();
  }
  for (int aLength = 0; aLength < myColorFormats.Length(); ++aLength)
  {
    myColorTextures.Append(aLength < aTextures.Length()
                             ? aTextures.Value(aLength)
                             : new OpenGl_Texture(myResourceId + ":color" + aLength));
  }

  myDepthFormat = theDepthStencilTexture->GetFormat();
  myNbSamples   = theNbSamples;
  if (theGlContext->arbFBO == NULL)
  {
    return false;
  }

  // clean up previous state
  Release(theGlContext.operator->());
  if (myColorFormats.IsEmpty() && myDepthFormat == 0)
  {
    return false;
  }

  myDepthStencilTexture = theDepthStencilTexture;
  myIsOwnColor          = true;
  myIsOwnDepth          = false;
  myIsOwnBuffer         = true;

  // setup viewport sizes as is
  myVPSizeX        = theSize.x();
  myVPSizeY        = theSize.y();
  const int aSizeX = theSize.x() > 0 ? theSize.x() : 2;
  const int aSizeY = theSize.y() > 0 ? theSize.y() : 2;

  // Create the textures (will be used as color buffer and depth-stencil buffer)
  if (theNbSamples != 0)
  {
    for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
      const GLint                        aColorFormat  = myColorFormats(aColorBufferIdx);
      if (aColorFormat == 0
          || !aColorTexture
                ->Init2DMultisample(theGlContext, theNbSamples, aColorFormat, aSizeX, aSizeY))
      {
        Release(theGlContext.get());
        return false;
      }
    }
  }
  else
  {
    for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
      const GLint                        aColorFormat  = myColorFormats(aColorBufferIdx);
      const OpenGl_TextureFormat         aFormat =
        OpenGl_TextureFormat::FindSizedFormat(theGlContext, aColorFormat);
      if (!aFormat.IsValid()
          || !aColorTexture->Init(theGlContext,
                                  aFormat,
                                  NCollection_Vec2<int>(aSizeX, aSizeY),
                                  Graphic3d_TypeOfTexture_2D))
      {
        Release(theGlContext.get());
        return false;
      }
    }
  }

  // Build FBO and setup it as texture
  theGlContext->arbFBO->glGenFramebuffers(1, &myGlFBufferId);
  theGlContext->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, myGlFBufferId);

  for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
  {
    const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
    if (aColorTexture->IsValid())
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + aColorBufferIdx,
                                                   aColorTexture->GetTarget(),
                                                   aColorTexture->TextureId(),
                                                   0);
    }
  }
  if (myDepthStencilTexture->IsValid())
  {
    if (hasDepthStencilAttach(theGlContext))
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_DEPTH_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_STENCIL_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
    }
  }
  if (theGlContext->arbFBO->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Release(theGlContext.operator->());
    return false;
  }

  UnbindBuffer(theGlContext);
  return true;
}

//=================================================================================================

bool OpenGl_FrameBuffer::Init(const occ::handle<OpenGl_Context>& theGlContext,
                              const NCollection_Vec2<int>&       theSize,
                              const NCollection_Vector<int>&     theColorFormats,
                              const int                          theDepthFormat,
                              const int                          theNbSamples)
{
  myColorFormats = theColorFormats;

  OpenGl_TextureArray aTextures(myColorTextures);
  if (!myColorTextures.IsEmpty())
  {
    for (OpenGl_TextureArray::Iterator aTextureIt(myColorTextures); aTextureIt.More();
         aTextureIt.Next())
    {
      aTextureIt.Value()->Release(theGlContext.operator->());
    }
    myColorTextures.Clear();
  }
  for (int aLength = 0; aLength < myColorFormats.Length(); ++aLength)
  {
    myColorTextures.Append(aLength < aTextures.Length()
                             ? aTextures.Value(aLength)
                             : new OpenGl_Texture(myResourceId + ":color" + aLength));
  }

  myDepthFormat = theDepthFormat;
  myNbSamples   = theNbSamples;
  myInitVPSizeX = theSize.x();
  myInitVPSizeY = theSize.y();
  if (theGlContext->arbFBO == NULL)
  {
    return false;
  }

  // clean up previous state
  Release(theGlContext.operator->());
  if (myColorFormats.IsEmpty() && myDepthFormat == 0)
  {
    return false;
  }

  if (theNbSamples != 0 && !theGlContext->HasTextureMultisampling()
      && theGlContext->MaxMsaaSamples() > 1)
  {
    return InitRenderBuffer(theGlContext, theSize, theColorFormats, theDepthFormat, theNbSamples);
  }

  myIsOwnColor  = true;
  myIsOwnBuffer = true;
  myIsOwnDepth  = true;

  // setup viewport sizes as is
  myVPSizeX              = theSize.x();
  myVPSizeY              = theSize.y();
  const int aSizeX       = theSize.x() > 0 ? theSize.x() : 2;
  const int aSizeY       = theSize.y() > 0 ? theSize.y() : 2;
  bool      hasStencilRB = false;

  // Create the textures (will be used as color buffer and depth-stencil buffer)
  if (theNbSamples != 0)
  {
    for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
      const GLint                        aColorFormat  = myColorFormats(aColorBufferIdx);
      if (aColorFormat == 0
          || !aColorTexture
                ->Init2DMultisample(theGlContext, theNbSamples, aColorFormat, aSizeX, aSizeY))
      {
        Release(theGlContext.operator->());
        return false;
      }
    }
    if (myDepthFormat != 0
        && !myDepthStencilTexture
              ->Init2DMultisample(theGlContext, theNbSamples, myDepthFormat, aSizeX, aSizeY))
    {
      Release(theGlContext.operator->());
      return false;
    }
  }
  else
  {
    for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
      const GLint                        aColorFormat  = myColorFormats(aColorBufferIdx);
      const OpenGl_TextureFormat         aFormat =
        OpenGl_TextureFormat::FindSizedFormat(theGlContext, aColorFormat);
      if (!aFormat.IsValid()
          || !aColorTexture->Init(theGlContext,
                                  aFormat,
                                  NCollection_Vec2<int>(aSizeX, aSizeY),
                                  Graphic3d_TypeOfTexture_2D))
      {
        Release(theGlContext.operator->());
        return false;
      }
    }

    // extensions (GL_OES_packed_depth_stencil, GL_OES_depth_texture) + GL version might be used to
    // determine supported formats instead of just trying to create such texture
    const OpenGl_TextureFormat aDepthFormat =
      OpenGl_TextureFormat::FindSizedFormat(theGlContext, myDepthFormat);
    if (aDepthFormat.IsValid()
        && !myDepthStencilTexture->Init(theGlContext,
                                        aDepthFormat,
                                        NCollection_Vec2<int>(aSizeX, aSizeY),
                                        Graphic3d_TypeOfTexture_2D))
    {
      theGlContext->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                                GL_DEBUG_TYPE_PORTABILITY,
                                0,
                                GL_DEBUG_SEVERITY_HIGH,
                                "Warning! Depth textures are not supported by hardware!");

      hasStencilRB = aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL && theGlContext->extPDS;
      GLint aDepthStencilFormat = hasStencilRB ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT16;

      theGlContext->arbFBO->glGenRenderbuffers(1, &myGlDepthRBufferId);
      theGlContext->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, myGlDepthRBufferId);
      theGlContext->arbFBO->glRenderbufferStorage(GL_RENDERBUFFER,
                                                  aDepthStencilFormat,
                                                  aSizeX,
                                                  aSizeY);
      theGlContext->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, NO_RENDERBUFFER);
      const GLenum aRendImgErr = theGlContext->core11fwd->glGetError();
      if (aRendImgErr != GL_NO_ERROR)
      {
        theGlContext->PushMessage(
          GL_DEBUG_SOURCE_APPLICATION,
          GL_DEBUG_TYPE_ERROR,
          0,
          GL_DEBUG_SEVERITY_HIGH,
          TCollection_AsciiString("Error: FBO depth render buffer ") + aSizeX + "x" + aSizeY
            + " IF: " + OpenGl_TextureFormat::FormatFormat(aDepthStencilFormat)
            + " can not be created with error " + OpenGl_Context::FormatGlError(aRendImgErr) + ".");
        Release(theGlContext.get());
        return false;
      }
    }
  }

  // Build FBO and setup it as texture
  theGlContext->arbFBO->glGenFramebuffers(1, &myGlFBufferId);
  theGlContext->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, myGlFBufferId);
  for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
  {
    const occ::handle<OpenGl_Texture>& aColorTexture = myColorTextures(aColorBufferIdx);
    if (aColorTexture->IsValid())
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + aColorBufferIdx,
                                                   aColorTexture->GetTarget(),
                                                   aColorTexture->TextureId(),
                                                   0);
    }
  }

  if (myDepthStencilTexture->IsValid())
  {
    if (hasDepthStencilAttach(theGlContext))
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_DEPTH_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
      theGlContext->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_STENCIL_ATTACHMENT,
                                                   myDepthStencilTexture->GetTarget(),
                                                   myDepthStencilTexture->TextureId(),
                                                   0);
    }
  }
  else if (myGlDepthRBufferId != NO_RENDERBUFFER)
  {
    if (hasDepthStencilAttach(theGlContext) && hasStencilRB)
    {
      theGlContext->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                      GL_DEPTH_STENCIL_ATTACHMENT,
                                                      GL_RENDERBUFFER,
                                                      myGlDepthRBufferId);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                      GL_DEPTH_ATTACHMENT,
                                                      GL_RENDERBUFFER,
                                                      myGlDepthRBufferId);
      if (hasStencilRB)
      {
        theGlContext->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                        GL_STENCIL_ATTACHMENT,
                                                        GL_RENDERBUFFER,
                                                        myGlDepthRBufferId);
      }
    }
  }
  if (theGlContext->arbFBO->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Release(theGlContext.operator->());
    return false;
  }

  UnbindBuffer(theGlContext);
  return true;
}

//=================================================================================================

bool OpenGl_FrameBuffer::InitLazy(const occ::handle<OpenGl_Context>& theGlContext,
                                  const NCollection_Vec2<int>&       theViewportSize,
                                  const int                          theColorFormat,
                                  const int                          theDepthFormat,
                                  const int                          theNbSamples)
{
  NCollection_Vector<int> aColorFormats;
  aColorFormats.Append(theColorFormat);
  return InitLazy(theGlContext, theViewportSize, aColorFormats, theDepthFormat, theNbSamples);
}

//=================================================================================================

bool OpenGl_FrameBuffer::InitLazy(const occ::handle<OpenGl_Context>& theGlContext,
                                  const NCollection_Vec2<int>&       theViewportSize,
                                  const NCollection_Vector<int>&     theColorFormats,
                                  const int                          theDepthFormat,
                                  const int                          theNbSamples)
{
  if (myVPSizeX == theViewportSize.x() && myVPSizeY == theViewportSize.y()
      && myColorFormats == theColorFormats && myDepthFormat == theDepthFormat
      && myNbSamples == theNbSamples)
  {
    return IsValid();
  }

  return Init(theGlContext, theViewportSize, theColorFormats, theDepthFormat, theNbSamples);
}

//=================================================================================================

bool OpenGl_FrameBuffer::InitWithRB(const occ::handle<OpenGl_Context>& theGlCtx,
                                    const NCollection_Vec2<int>&       theSize,
                                    const int                          theColorFormat,
                                    const int                          theDepthFormat,
                                    const unsigned int                 theColorRBufferFromWindow)
{
  NCollection_Vector<int> aColorFormats;
  if (theColorFormat != 0)
  {
    aColorFormats.Append(theColorFormat);
  }
  return initRenderBuffer(theGlCtx,
                          theSize,
                          aColorFormats,
                          theDepthFormat,
                          0,
                          theColorRBufferFromWindow);
}

//=================================================================================================

bool OpenGl_FrameBuffer::initRenderBuffer(const occ::handle<OpenGl_Context>& theGlCtx,
                                          const NCollection_Vec2<int>&       theSize,
                                          const NCollection_Vector<int>&     theColorFormats,
                                          const int                          theDepthFormat,
                                          const int                          theNbSamples,
                                          const unsigned int theColorRBufferFromWindow)
{
  myColorFormats = theColorFormats;
  if (!myColorTextures.IsEmpty())
  {
    occ::handle<OpenGl_Texture> aTexutre = myColorTextures.First();
    for (OpenGl_TextureArray::Iterator aTextureIt(myColorTextures); aTextureIt.More();
         aTextureIt.Next())
    {
      aTextureIt.Value()->Release(theGlCtx.operator->());
    }
    myColorTextures.Clear();
    myColorTextures.Append(aTexutre);
  }

  myDepthFormat = theDepthFormat;
  myNbSamples   = theNbSamples;
  myInitVPSizeX = theSize.x();
  myInitVPSizeY = theSize.y();
  if (theGlCtx->arbFBO == NULL)
  {
    return false;
  }

  // clean up previous state
  Release(theGlCtx.operator->());
  if (theNbSamples > theGlCtx->MaxMsaaSamples() || theNbSamples < 0)
  {
    theGlCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          TCollection_AsciiString("Error: FBO creation failed - MSAA")
                            + theNbSamples + " render buffer exceeds samples limit: "
                            + theGlCtx->MaxMsaaSamples() + ").");
    return false;
  }

  myIsOwnColor  = true;
  myIsOwnBuffer = true;
  myIsOwnDepth  = true;

  // setup viewport sizes as is
  myVPSizeX        = theSize.x();
  myVPSizeY        = theSize.y();
  const int aSizeX = theSize.x() > 0 ? theSize.x() : 2;
  const int aSizeY = theSize.y() > 0 ? theSize.y() : 2;

  // Create the render-buffers
  if (theColorRBufferFromWindow != NO_RENDERBUFFER)
  {
    myGlColorRBufferId = theColorRBufferFromWindow;
  }
  else if (!theColorFormats.IsEmpty())
  {
    if (theColorFormats.Size() != 1)
    {
      throw Standard_NotImplemented(
        "Multiple color attachments as FBO render buffers are not implemented");
    }

    const GLint                aColorFormat = theColorFormats.First();
    const OpenGl_TextureFormat aFormat =
      OpenGl_TextureFormat::FindSizedFormat(theGlCtx, aColorFormat);
    if (!aFormat.IsValid())
    {
      Release(theGlCtx.operator->());
      return false;
    }

    theGlCtx->arbFBO->glGenRenderbuffers(1, &myGlColorRBufferId);
    theGlCtx->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, myGlColorRBufferId);
    if (theNbSamples != 0)
    {
      theGlCtx->Functions()->glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                                                              theNbSamples,
                                                              aFormat.InternalFormat(),
                                                              aSizeX,
                                                              aSizeY);
    }
    else
    {
      theGlCtx->arbFBO->glRenderbufferStorage(GL_RENDERBUFFER,
                                              aFormat.InternalFormat(),
                                              aSizeX,
                                              aSizeY);
    }
    theGlCtx->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, NO_RENDERBUFFER);

    const GLenum aRendImgErr = theGlCtx->core11fwd->glGetError();
    if (aRendImgErr != GL_NO_ERROR)
    {
      theGlCtx->PushMessage(
        GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_ERROR,
        0,
        GL_DEBUG_SEVERITY_HIGH,
        TCollection_AsciiString("Error: FBO color render buffer ") + aSizeX + "x" + aSizeY + "@"
          + theNbSamples + " IF: " + OpenGl_TextureFormat::FormatFormat(aFormat.InternalFormat())
          + " can not be created with error " + OpenGl_Context::FormatGlError(aRendImgErr) + ".");
      Release(theGlCtx.get());
      return false;
    }
  }

  bool hasStencilRB = false;
  if (myDepthFormat != 0)
  {
    const OpenGl_TextureFormat aDepthFormat =
      OpenGl_TextureFormat::FindSizedFormat(theGlCtx, myDepthFormat);
    hasStencilRB = aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL;

    theGlCtx->arbFBO->glGenRenderbuffers(1, &myGlDepthRBufferId);
    theGlCtx->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, myGlDepthRBufferId);
    if (theNbSamples != 0)
    {
      theGlCtx->Functions()->glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                                                              theNbSamples,
                                                              myDepthFormat,
                                                              aSizeX,
                                                              aSizeY);
    }
    else
    {
      theGlCtx->arbFBO->glRenderbufferStorage(GL_RENDERBUFFER, myDepthFormat, aSizeX, aSizeY);
    }
    theGlCtx->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, NO_RENDERBUFFER);

    const GLenum aRendImgErr = theGlCtx->core11fwd->glGetError();
    if (aRendImgErr != GL_NO_ERROR)
    {
      theGlCtx->PushMessage(
        GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_ERROR,
        0,
        GL_DEBUG_SEVERITY_HIGH,
        TCollection_AsciiString("Error: FBO depth render buffer ") + aSizeX + "x" + aSizeY + "@"
          + theNbSamples + " IF: " + OpenGl_TextureFormat::FormatFormat(myDepthFormat)
          + " can not be created with error " + OpenGl_Context::FormatGlError(aRendImgErr) + ".");
      Release(theGlCtx.get());
      return false;
    }
  }

  // create FBO
  theGlCtx->arbFBO->glGenFramebuffers(1, &myGlFBufferId);
  theGlCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, myGlFBufferId);
  if (myGlColorRBufferId != NO_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                GL_COLOR_ATTACHMENT0,
                                                GL_RENDERBUFFER,
                                                myGlColorRBufferId);
  }
  if (myGlDepthRBufferId != NO_RENDERBUFFER)
  {
    if (hasDepthStencilAttach(theGlCtx) && hasStencilRB)
    {
      theGlCtx->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                                  GL_RENDERBUFFER,
                                                  myGlDepthRBufferId);
    }
    else
    {
      theGlCtx->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                  GL_DEPTH_ATTACHMENT,
                                                  GL_RENDERBUFFER,
                                                  myGlDepthRBufferId);
      if (hasStencilRB)
      {
        theGlCtx->arbFBO->glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                    GL_STENCIL_ATTACHMENT,
                                                    GL_RENDERBUFFER,
                                                    myGlDepthRBufferId);
      }
    }
  }
  if (theGlCtx->arbFBO->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    UnbindBuffer(theGlCtx);
    Release(theGlCtx.operator->());
    return false;
  }

  UnbindBuffer(theGlCtx);
  return true;
}

//=================================================================================================

bool OpenGl_FrameBuffer::InitWrapper(const occ::handle<OpenGl_Context>& theGlCtx)
{
  myNbSamples = 0;
  if (theGlCtx->arbFBO == NULL)
  {
    return false;
  }

  // clean up previous state
  Release(theGlCtx.operator->());

  GLint anFbo = GLint(NO_FRAMEBUFFER);
  theGlCtx->core11fwd->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &anFbo);
  if (anFbo == GLint(NO_FRAMEBUFFER))
  {
    return false;
  }

  GLint aColorType = 0;
  GLint aColorId   = 0;
  GLint aDepthType = 0;
  GLint aDepthId   = 0;
  theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                          GL_COLOR_ATTACHMENT0,
                                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                                          &aColorType);
  theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                          GL_DEPTH_ATTACHMENT,
                                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                                          &aDepthType);

  myGlFBufferId = GLuint(anFbo);
  myIsOwnColor  = false;
  myIsOwnBuffer = false;
  myIsOwnDepth  = false;
  if (aColorType == GL_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                            GL_COLOR_ATTACHMENT0,
                                                            GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                            &aColorId);
    myGlColorRBufferId = aColorId;
  }
  else if (aColorType == GL_TEXTURE)
  {
    // myColorTextures[0]->InitWrapper() - not implemented, just skip it
  }
  else if (aColorType != GL_NONE)
  {
    theGlCtx->PushMessage(
      GL_DEBUG_SOURCE_APPLICATION,
      GL_DEBUG_TYPE_ERROR,
      0,
      GL_DEBUG_SEVERITY_HIGH,
      "OpenGl_FrameBuffer::InitWrapper(), color attachment of unsupported type has been skipped!");
  }

  if (aDepthType == GL_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                            GL_DEPTH_ATTACHMENT,
                                                            GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                            &aDepthId);
    myGlDepthRBufferId = aDepthId;
  }
  else if (aDepthType == GL_TEXTURE)
  {
    // myDepthStencilTexture->InitWrapper() - not implemented, just skip it
  }
  else if (aDepthType != GL_NONE)
  {
    theGlCtx->PushMessage(
      GL_DEBUG_SOURCE_APPLICATION,
      GL_DEBUG_TYPE_ERROR,
      0,
      GL_DEBUG_SEVERITY_HIGH,
      "OpenGl_FrameBuffer::InitWrapper(), depth attachment of unsupported type has been skipped!");
  }

  // retrieve dimensions
  GLuint aRBuffer = myGlColorRBufferId != NO_RENDERBUFFER ? myGlColorRBufferId : myGlDepthRBufferId;
  if (aRBuffer != NO_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, aRBuffer);
    theGlCtx->arbFBO->glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                                   GL_RENDERBUFFER_WIDTH,
                                                   &myInitVPSizeX);
    theGlCtx->arbFBO->glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                                   GL_RENDERBUFFER_HEIGHT,
                                                   &myInitVPSizeY);
    theGlCtx->arbFBO->glBindRenderbuffer(GL_RENDERBUFFER, NO_RENDERBUFFER);
  }

  GLint aViewport[4] = {};
  theGlCtx->core11fwd->glGetIntegerv(GL_VIEWPORT, aViewport);
  myVPSizeX = aViewport[2];
  myVPSizeY = aViewport[3];
  return true;
}

//=================================================================================================

void OpenGl_FrameBuffer::Release(OpenGl_Context* theGlCtx)
{
  if (isValidFrameBuffer())
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN(
      !myIsOwnBuffer || theGlCtx != NULL,
      "OpenGl_FrameBuffer destroyed without GL context! Possible GPU memory leakage...", );
    if (myIsOwnBuffer && theGlCtx->IsValid())
    {
      theGlCtx->arbFBO->glDeleteFramebuffers(1, &myGlFBufferId);
      if (myGlColorRBufferId != NO_RENDERBUFFER)
      {
        theGlCtx->arbFBO->glDeleteRenderbuffers(1, &myGlColorRBufferId);
      }
      if (myGlDepthRBufferId != NO_RENDERBUFFER)
      {
        theGlCtx->arbFBO->glDeleteRenderbuffers(1, &myGlDepthRBufferId);
      }
    }
    myGlFBufferId      = NO_FRAMEBUFFER;
    myGlColorRBufferId = NO_RENDERBUFFER;
    myGlDepthRBufferId = NO_RENDERBUFFER;
    myIsOwnBuffer      = false;
  }

  if (myIsOwnColor)
  {
    for (int aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      myColorTextures(aColorBufferIdx)->Release(theGlCtx);
    }
    myIsOwnColor = false;
  }

  if (myIsOwnDepth)
  {
    myDepthStencilTexture->Release(theGlCtx);
    myIsOwnDepth = false;
  }

  myVPSizeX = 0;
  myVPSizeY = 0;
}

//=================================================================================================

void OpenGl_FrameBuffer::SetupViewport(const occ::handle<OpenGl_Context>& theGlCtx)
{
  const int aViewport[4] = {0, 0, myVPSizeX, myVPSizeY};
  theGlCtx->ResizeViewport(aViewport);
}

//=================================================================================================

void OpenGl_FrameBuffer::ChangeViewport(const int theVPSizeX, const int theVPSizeY)
{
  myVPSizeX = theVPSizeX;
  myVPSizeY = theVPSizeY;
}

//=================================================================================================

void OpenGl_FrameBuffer::BindBuffer(const occ::handle<OpenGl_Context>& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, myGlFBufferId);
  theGlCtx->SetFrameBufferSRGB(true);
}

//=================================================================================================

void OpenGl_FrameBuffer::BindDrawBuffer(const occ::handle<OpenGl_Context>& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, myGlFBufferId);
  theGlCtx->SetFrameBufferSRGB(true);
}

//=================================================================================================

void OpenGl_FrameBuffer::BindReadBuffer(const occ::handle<OpenGl_Context>& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer(GL_READ_FRAMEBUFFER, myGlFBufferId);
}

//=================================================================================================

void OpenGl_FrameBuffer::UnbindBuffer(const occ::handle<OpenGl_Context>& theGlCtx)
{
  if (!theGlCtx->DefaultFrameBuffer().IsNull()
      && theGlCtx->DefaultFrameBuffer().operator->() != this)
  {
    theGlCtx->DefaultFrameBuffer()->BindBuffer(theGlCtx);
  }
  else
  {
    theGlCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, NO_FRAMEBUFFER);
    theGlCtx->SetFrameBufferSRGB(false);
  }
}

//=================================================================================================

inline size_t getAligned(const size_t theNumber, const size_t theAlignment)
{
  return theNumber + theAlignment - 1 - (theNumber - 1) % theAlignment;
}

template <typename T>
inline void convertRowFromRgba(T*                     theRgbRow,
                               const Image_ColorRGBA* theRgbaRow,
                               const size_t           theWidth)
{
  for (size_t aCol = 0; aCol < theWidth; ++aCol)
  {
    const Image_ColorRGBA& anRgba = theRgbaRow[aCol];
    T&                     anRgb  = theRgbRow[aCol];
    anRgb.r()                     = anRgba.r();
    anRgb.g()                     = anRgba.g();
    anRgb.b()                     = anRgba.b();
  }
}

//=================================================================================================

bool OpenGl_FrameBuffer::BufferDump(const occ::handle<OpenGl_Context>&     theGlCtx,
                                    const occ::handle<OpenGl_FrameBuffer>& theFbo,
                                    Image_PixMap&                          theImage,
                                    Graphic3d_BufferType                   theBufferType)
{
  if (theGlCtx.IsNull() || theImage.IsEmpty())
  {
    return false;
  }

  GLenum aFormat        = 0;
  GLenum aType          = 0;
  bool   toSwapRgbaBgra = false;
  bool   toConvRgba2Rgb = false;
  switch (theImage.Format())
  {
    case Image_Format_Gray: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return false;
      }

      aFormat = theBufferType == Graphic3d_BT_Depth || theBufferType == Graphic3d_BT_ShadowMap
                  ? GL_DEPTH_COMPONENT
                  : GL_RED;
      aType   = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_Format_Gray16: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return false;
      }

      aFormat = theBufferType == Graphic3d_BT_Depth || theBufferType == Graphic3d_BT_ShadowMap
                  ? GL_DEPTH_COMPONENT
                  : GL_RED;
      aType   = GL_UNSIGNED_SHORT;
      break;
    }
    case Image_Format_GrayF: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return false;
      }

      aFormat = theBufferType == Graphic3d_BT_Depth || theBufferType == Graphic3d_BT_ShadowMap
                  ? GL_DEPTH_COMPONENT
                  : GL_RED;
      aType   = GL_FLOAT;
      break;
    }
    case Image_Format_RGF: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return false;
      }

      aFormat = GL_RG;
      aType   = GL_FLOAT;
      break;
    }
    case Image_Format_RGB: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat        = GL_RGBA;
        toConvRgba2Rgb = true;
      }
      else
      {
        aFormat = GL_RGB;
      }
      aType = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_Format_BGR: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat        = GL_RGBA;
        toConvRgba2Rgb = true;
      }
      else
      {
        aFormat = GL_BGR;
      }
      aType = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_Format_BGRA:
    case Image_Format_BGR32: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat        = GL_RGBA;
        toSwapRgbaBgra = true;
      }
      else
      {
        aFormat = GL_BGRA;
      }
      aType = GL_UNSIGNED_BYTE;
      break;
    }
    case Image_Format_BGRF: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return false;
      }

      aFormat = GL_BGR;
      aType   = GL_FLOAT;
      break;
    }
    case Image_Format_BGRAF: {
      if (theGlCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return false;
      }

      aFormat = GL_BGRA;
      aType   = GL_FLOAT;
      break;
    }
    case Image_Format_RGBA:
    case Image_Format_RGB32:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_RGBF:
      aFormat = GL_RGB;
      aType   = GL_FLOAT;
      break;
    case Image_Format_RGBAF:
      aFormat = GL_RGBA;
      aType   = GL_FLOAT;
      break;
    case Image_Format_RGBAF_half:
      aFormat = GL_RGBA;
      aType   = GL_HALF_FLOAT;
      break;
    case Image_Format_Alpha:
    case Image_Format_AlphaF:
      return false; // GL_ALPHA is no more supported in core context
    case Image_Format_GrayF_half:
    case Image_Format_RGF_half:
    case Image_Format_UNKNOWN:
      return false;
  }

  if (aFormat == 0)
  {
    return false;
  }

  GLint aReadBufferPrev = GL_BACK;
  if (theGlCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES
      && (theBufferType == Graphic3d_BT_Depth || theBufferType == Graphic3d_BT_ShadowMap)
      && aFormat != GL_DEPTH_COMPONENT)
  {
    return false;
  }

  // bind FBO if used
  if (!theFbo.IsNull() && theFbo->IsValid())
  {
    theFbo->BindBuffer(theGlCtx);
  }
  else if (theGlCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
  {
    theGlCtx->core11fwd->glGetIntegerv(GL_READ_BUFFER, &aReadBufferPrev);
    GLint aDrawBufferPrev = GL_BACK;
    theGlCtx->core11fwd->glGetIntegerv(GL_DRAW_BUFFER, &aDrawBufferPrev);
    theGlCtx->core11fwd->glReadBuffer(aDrawBufferPrev);
  }

  // setup alignment
  // clang-format off
  const GLint anAligment = std::min(GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL
  // clang-format on
  theGlCtx->core11fwd->glPixelStorei(GL_PACK_ALIGNMENT, anAligment);
  bool isBatchCopy = !theImage.IsTopDown();

  const GLint anExtraBytes       = GLint(theImage.RowExtraBytes());
  GLint       aPixelsWidth       = GLint(theImage.SizeRowBytes() / theImage.SizePixelBytes());
  size_t      aSizeRowBytesEstim = getAligned(theImage.SizePixelBytes() * aPixelsWidth, anAligment);
  if (anExtraBytes < anAligment)
  {
    aPixelsWidth = 0;
  }
  else if (aSizeRowBytesEstim != theImage.SizeRowBytes())
  {
    aPixelsWidth = 0;
    isBatchCopy  = false;
  }
  if (theGlCtx->hasPackRowLength)
  {
    theGlCtx->core11fwd->glPixelStorei(GL_PACK_ROW_LENGTH, aPixelsWidth);
  }
  else if (aPixelsWidth != 0)
  {
    isBatchCopy = false;
  }

  if (toConvRgba2Rgb)
  {
    const size_t       aRowSize = theImage.SizeX() * 4;
    NCollection_Buffer aRowBuffer(Image_PixMap::DefaultAllocator());
    if (!aRowBuffer.Allocate(aRowSize))
    {
      return false;
    }

    for (size_t aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      // Image_PixMap rows indexation always starts from the upper corner
      // while order in memory depends on the flag and processed by ChangeRow() method
      theGlCtx->core11fwd->glReadPixels(0,
                                        GLint(theImage.SizeY() - aRow - 1),
                                        GLsizei(theImage.SizeX()),
                                        1,
                                        aFormat,
                                        aType,
                                        aRowBuffer.ChangeData());
      const Image_ColorRGBA* aRowDataRgba = (const Image_ColorRGBA*)aRowBuffer.Data();
      if (theImage.Format() == Image_Format_BGR)
      {
        convertRowFromRgba((Image_ColorBGR*)theImage.ChangeRow(aRow),
                           aRowDataRgba,
                           theImage.SizeX());
      }
      else
      {
        convertRowFromRgba((Image_ColorRGB*)theImage.ChangeRow(aRow),
                           aRowDataRgba,
                           theImage.SizeX());
      }
    }
  }
  else if (!isBatchCopy)
  {
    // copy row by row
    for (size_t aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      // Image_PixMap rows indexation always starts from the upper corner
      // while order in memory depends on the flag and processed by ChangeRow() method
      theGlCtx->core11fwd->glReadPixels(0,
                                        GLint(theImage.SizeY() - aRow - 1),
                                        GLsizei(theImage.SizeX()),
                                        1,
                                        aFormat,
                                        aType,
                                        theImage.ChangeRow(aRow));
    }
  }
  else
  {
    theGlCtx->core11fwd->glReadPixels(0,
                                      0,
                                      GLsizei(theImage.SizeX()),
                                      GLsizei(theImage.SizeY()),
                                      aFormat,
                                      aType,
                                      theImage.ChangeData());
  }
  const bool hasErrors = theGlCtx->ResetErrors(true);

  theGlCtx->core11fwd->glPixelStorei(GL_PACK_ALIGNMENT, 1);
  if (theGlCtx->hasPackRowLength)
  {
    theGlCtx->core11fwd->glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  }

  if (!theFbo.IsNull() && theFbo->IsValid())
  {
    theFbo->UnbindBuffer(theGlCtx);
  }
  else if (theGlCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
  {
    theGlCtx->core11fwd->glReadBuffer(aReadBufferPrev);
  }

  if (toSwapRgbaBgra)
  {
    Image_PixMap::SwapRgbaBgra(theImage);
  }

  return !hasErrors;
}

//=================================================================================================

size_t OpenGl_FrameBuffer::EstimatedDataSize() const
{
  if (!IsValid())
  {
    return 0;
  }

  size_t aSize = 0;
  for (OpenGl_TextureArray::Iterator aTextureIt(myColorTextures); aTextureIt.More();
       aTextureIt.Next())
  {
    aSize += aTextureIt.Value()->EstimatedDataSize();
  }
  if (!myDepthStencilTexture.IsNull())
  {
    aSize += myDepthStencilTexture->EstimatedDataSize();
  }
  if (myGlColorRBufferId != NO_RENDERBUFFER && !myColorFormats.IsEmpty())
  {
    aSize += OpenGl_Texture::PixelSizeOfPixelFormat(myColorFormats.First()) * myInitVPSizeX
             * myInitVPSizeY;
  }
  if (myGlDepthRBufferId != NO_RENDERBUFFER)
  {
    aSize += OpenGl_Texture::PixelSizeOfPixelFormat(myDepthFormat) * myInitVPSizeX * myInitVPSizeY;
  }
  return aSize;
}
