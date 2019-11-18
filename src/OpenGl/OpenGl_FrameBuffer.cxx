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

#include <NCollection_AlignedAllocator.hxx>
#include <Standard_Assert.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_FrameBuffer,OpenGl_Resource)

namespace
{
  //! Checks whether two format arrays are equal or not.
  static bool operator== (const OpenGl_ColorFormats& theFmt1,
                          const OpenGl_ColorFormats& theFmt2)
  {
    if (theFmt1.Length() != theFmt2.Length())
      return false;
    OpenGl_ColorFormats::Iterator anIt1 (theFmt1);
    OpenGl_ColorFormats::Iterator anIt2 (theFmt1);
    for (; anIt1.More(); anIt1.Next(), anIt2.Next())
    {
      if (anIt1.Value() != anIt2.Value())
        return false;
    }
    return true;
  }

  //! Return TRUE if GL_DEPTH_STENCIL_ATTACHMENT can be used.
  static bool hasDepthStencilAttach (const Handle(OpenGl_Context)& theCtx)
  {
  #ifdef __EMSCRIPTEN__
    // supported since WebGL 2.0,
    // while WebGL 1.0 + GL_WEBGL_depth_texture needs GL_DEPTH_STENCIL_ATTACHMENT
    // and NOT separate GL_DEPTH_ATTACHMENT+GL_STENCIL_ATTACHMENT calls which is different to OpenGL ES 2.0 + extension
    return theCtx->IsGlGreaterEqual (3, 0) || theCtx->extPDS;
  #elif defined(GL_ES_VERSION_2_0)
    // supported since OpenGL ES 3.0,
    // while OpenGL ES 2.0 + GL_EXT_packed_depth_stencil needs separate GL_DEPTH_ATTACHMENT+GL_STENCIL_ATTACHMENT calls
    return theCtx->IsGlGreaterEqual (3, 0);
  #else
    // available on desktop since OpenGL 3.0
    // or OpenGL 2.0 + GL_ARB_framebuffer_object (GL_EXT_framebuffer_object is unsupported by OCCT)
    (void )theCtx;
    return true;
  #endif
  }
}

// =======================================================================
// function : OpenGl_FrameBuffer
// purpose  :
// =======================================================================
OpenGl_FrameBuffer::OpenGl_FrameBuffer()
: myInitVPSizeX (0),
  myInitVPSizeY (0),
  myVPSizeX (0),
  myVPSizeY (0),
  myNbSamples (0),
  myDepthFormat (GL_DEPTH24_STENCIL8),
  myGlFBufferId (NO_FRAMEBUFFER),
  myGlColorRBufferId (NO_RENDERBUFFER),
  myGlDepthRBufferId (NO_RENDERBUFFER),
  myIsOwnBuffer  (false),
  myIsOwnDepth  (false),
  myDepthStencilTexture (new OpenGl_Texture())
{
  myColorFormats.Append (GL_RGBA8);
  myColorTextures.Append (new OpenGl_Texture());
}

// =======================================================================
// function : ~OpenGl_FrameBuffer
// purpose  :
// =======================================================================
OpenGl_FrameBuffer::~OpenGl_FrameBuffer()
{
  Release (NULL);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::Init (const Handle(OpenGl_Context)& theGlContext,
                                           const GLsizei                 theSizeX,
                                           const GLsizei                 theSizeY,
                                           const GLint                   theColorFormat,
                                           const GLint                   theDepthFormat,
                                           const GLsizei                 theNbSamples)
{
  OpenGl_ColorFormats aColorFormats;

  aColorFormats.Append (theColorFormat);

  return Init (theGlContext, theSizeX, theSizeY, aColorFormats, theDepthFormat, theNbSamples);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::Init (const Handle(OpenGl_Context)& theGlContext,
                                           const GLsizei                 theSizeX,
                                           const GLsizei                 theSizeY,
                                           const OpenGl_ColorFormats&    theColorFormats,
                                           const Handle(OpenGl_Texture)& theDepthStencilTexture,
                                           const GLsizei                 theNbSamples)
{
  myColorFormats = theColorFormats;

  OpenGl_TextureArray aTextures (myColorTextures);
  if (!myColorTextures.IsEmpty())
  {
    for (OpenGl_TextureArray::Iterator aTextureIt (myColorTextures); aTextureIt.More(); aTextureIt.Next())
    {
      aTextureIt.Value()->Release (theGlContext.operator->());
    }
    myColorTextures.Clear();
  }
  for (Standard_Integer aLength = 0; aLength < myColorFormats.Length(); ++aLength)
  {
    myColorTextures.Append (aLength < aTextures.Length() ? aTextures.Value (aLength) : new OpenGl_Texture());
  }

  myDepthFormat = theDepthStencilTexture->GetFormat();
  myNbSamples   = theNbSamples;
  if (theGlContext->arbFBO == NULL)
  {
    return Standard_False;
  }

  // clean up previous state
  Release (theGlContext.operator->());
  if (myColorFormats.IsEmpty()
   && myDepthFormat == 0)
  {
    return Standard_False;
  }

  myDepthStencilTexture = theDepthStencilTexture;
  myIsOwnDepth  = false;
  myIsOwnBuffer = true;

  // setup viewport sizes as is
  myVPSizeX = theSizeX;
  myVPSizeY = theSizeY;
  const Standard_Integer aSizeX = theSizeX > 0 ? theSizeX : 2;
  const Standard_Integer aSizeY = theSizeY > 0 ? theSizeY : 2;

  // Create the textures (will be used as color buffer and depth-stencil buffer)
  if (theNbSamples != 0)
  {
    for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const Handle(OpenGl_Texture)& aColorTexture = myColorTextures (aColorBufferIdx);
      const GLint                   aColorFormat  = myColorFormats  (aColorBufferIdx);
      if (aColorFormat != 0
      && !aColorTexture->Init2DMultisample (theGlContext, theNbSamples,
                                            aColorFormat, aSizeX, aSizeY))
      {
        Release (theGlContext.get());
        return Standard_False;
      }
    }
  }
  else
  {
    for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const Handle(OpenGl_Texture)& aColorTexture = myColorTextures (aColorBufferIdx);
      const GLint                   aColorFormat  = myColorFormats  (aColorBufferIdx);
      const OpenGl_TextureFormat aFormat = OpenGl_TextureFormat::FindSizedFormat (theGlContext, aColorFormat);
      if (aFormat.IsValid()
      && !aColorTexture->Init (theGlContext, aFormat, Graphic3d_Vec2i (aSizeX, aSizeY), Graphic3d_TOT_2D))
      {
        Release (theGlContext.get());
        return Standard_False;
      }
    }
  }

  // Build FBO and setup it as texture
  theGlContext->arbFBO->glGenFramebuffers (1, &myGlFBufferId);
  theGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, myGlFBufferId);

  for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
  {
    const Handle(OpenGl_Texture)& aColorTexture = myColorTextures (aColorBufferIdx);
    if (aColorTexture->IsValid())
    {
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + aColorBufferIdx,
                                                    aColorTexture->GetTarget(), aColorTexture->TextureId(), 0);
    }
  }
  if (myDepthStencilTexture->IsValid())
  {
    if (hasDepthStencilAttach (theGlContext))
    {
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                                    myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                    myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                                    myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
    }
  }
  if (theGlContext->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Release (theGlContext.operator->());
    return Standard_False;
  }

  UnbindBuffer (theGlContext);
  return Standard_True;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::Init (const Handle(OpenGl_Context)& theGlContext,
                                           const GLsizei                 theSizeX,
                                           const GLsizei                 theSizeY,
                                           const OpenGl_ColorFormats&    theColorFormats,
                                           const GLint                   theDepthFormat,
                                           const GLsizei                 theNbSamples)
{
  myColorFormats = theColorFormats;

  OpenGl_TextureArray aTextures (myColorTextures);
  if (!myColorTextures.IsEmpty())
  {
    for (OpenGl_TextureArray::Iterator aTextureIt (myColorTextures); aTextureIt.More(); aTextureIt.Next())
    {
      aTextureIt.Value()->Release (theGlContext.operator->());
    }
    myColorTextures.Clear();
  }
  for (Standard_Integer aLength = 0; aLength < myColorFormats.Length(); ++aLength)
  {
    myColorTextures.Append (aLength < aTextures.Length() ? aTextures.Value (aLength) : new OpenGl_Texture());
  }

  myDepthFormat = theDepthFormat;
  myNbSamples   = theNbSamples;
  myInitVPSizeX = theSizeX;
  myInitVPSizeY = theSizeY;
  if (theGlContext->arbFBO == NULL)
  {
    return Standard_False;
  }

  // clean up previous state
  Release (theGlContext.operator->());
  if (myColorFormats.IsEmpty()
   && myDepthFormat == 0)
  {
    return Standard_False;
  }

  myIsOwnBuffer = true;
  myIsOwnDepth  = true;

  // setup viewport sizes as is
  myVPSizeX = theSizeX;
  myVPSizeY = theSizeY;
  const Standard_Integer aSizeX = theSizeX > 0 ? theSizeX : 2;
  const Standard_Integer aSizeY = theSizeY > 0 ? theSizeY : 2;
  bool hasStencilRB = false;

  // Create the textures (will be used as color buffer and depth-stencil buffer)
  if (theNbSamples != 0)
  {
    for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const Handle(OpenGl_Texture)& aColorTexture = myColorTextures (aColorBufferIdx);
      const GLint                   aColorFormat  = myColorFormats  (aColorBufferIdx);
      if (aColorFormat != 0
      && !aColorTexture->Init2DMultisample (theGlContext, theNbSamples, aColorFormat, aSizeX, aSizeY))
      {
        Release (theGlContext.operator->());
        return Standard_False;
      }
    }
    if (myDepthFormat != 0
    && !myDepthStencilTexture->Init2DMultisample (theGlContext, theNbSamples, myDepthFormat, aSizeX, aSizeY))
    {
      Release (theGlContext.operator->());
      return Standard_False;
    }
  }
  else
  {
    for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
    {
      const Handle(OpenGl_Texture)& aColorTexture = myColorTextures (aColorBufferIdx);
      const GLint                   aColorFormat  = myColorFormats  (aColorBufferIdx);
      const OpenGl_TextureFormat aFormat = OpenGl_TextureFormat::FindSizedFormat (theGlContext, aColorFormat);
      if (aFormat.IsValid()
      && !aColorTexture->Init (theGlContext, aFormat, Graphic3d_Vec2i (aSizeX, aSizeY), Graphic3d_TOT_2D))
      {
        Release (theGlContext.operator->());
        return Standard_False;
      }
    }

    // extensions (GL_OES_packed_depth_stencil, GL_OES_depth_texture) + GL version might be used to determine supported formats
    // instead of just trying to create such texture
    const OpenGl_TextureFormat aDepthFormat = OpenGl_TextureFormat::FindSizedFormat (theGlContext, myDepthFormat);
    if (aDepthFormat.IsValid()
    && !myDepthStencilTexture->Init (theGlContext, aDepthFormat, Graphic3d_Vec2i (aSizeX, aSizeY), Graphic3d_TOT_2D))
    {
      theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                                 "Warning! Depth textures are not supported by hardware!");

      hasStencilRB = aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL
                  && theGlContext->extPDS;
      GLint aDepthStencilFormat = hasStencilRB
                                ? GL_DEPTH24_STENCIL8
                                : GL_DEPTH_COMPONENT16;

      theGlContext->arbFBO->glGenRenderbuffers (1, &myGlDepthRBufferId);
      theGlContext->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, myGlDepthRBufferId);
      theGlContext->arbFBO->glRenderbufferStorage (GL_RENDERBUFFER, aDepthStencilFormat, aSizeX, aSizeY);
      theGlContext->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, NO_RENDERBUFFER);
    }
  }

  // Build FBO and setup it as texture
  theGlContext->arbFBO->glGenFramebuffers (1, &myGlFBufferId);
  theGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, myGlFBufferId);
  for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
  {
    const Handle(OpenGl_Texture)& aColorTexture = myColorTextures (aColorBufferIdx);
    if (aColorTexture->IsValid())
    {
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + aColorBufferIdx,
                                                    aColorTexture->GetTarget(), aColorTexture->TextureId(), 0);
    }
  }

  if (myDepthStencilTexture->IsValid())
  {
    if (hasDepthStencilAttach (theGlContext))
    {
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                                    myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                    myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
      theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                                    myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
    }
  }
  else if (myGlDepthRBufferId != NO_RENDERBUFFER)
  {
    if (hasDepthStencilAttach (theGlContext) && hasStencilRB)
    {
      theGlContext->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                                       GL_RENDERBUFFER, myGlDepthRBufferId);
    }
    else
    {
      theGlContext->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                       GL_RENDERBUFFER, myGlDepthRBufferId);
      if (hasStencilRB)
      {
        theGlContext->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                                         GL_RENDERBUFFER, myGlDepthRBufferId);
      }
    }
  }
  if (theGlContext->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Release (theGlContext.operator->());
    return Standard_False;
  }

  UnbindBuffer (theGlContext);
  return Standard_True;
}

// =======================================================================
// function : InitLazy
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::InitLazy (const Handle(OpenGl_Context)& theGlContext,
                                               const GLsizei                 theViewportSizeX,
                                               const GLsizei                 theViewportSizeY,
                                               const GLint                   theColorFormat,
                                               const GLint                   theDepthFormat,
                                               const GLsizei                 theNbSamples)
{
  OpenGl_ColorFormats aColorFormats;

  aColorFormats.Append (theColorFormat);

  return InitLazy (theGlContext, theViewportSizeX, theViewportSizeY, aColorFormats, theDepthFormat, theNbSamples);
}

// =======================================================================
// function : InitLazy
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::InitLazy (const Handle(OpenGl_Context)& theGlContext,
                                               const GLsizei                 theViewportSizeX,
                                               const GLsizei                 theViewportSizeY,
                                               const OpenGl_ColorFormats&    theColorFormats,
                                               const GLint                   theDepthFormat,
                                               const GLsizei                 theNbSamples)
{
  if (myVPSizeX      == theViewportSizeX
   && myVPSizeY      == theViewportSizeY
   && myColorFormats == theColorFormats
   && myDepthFormat  == theDepthFormat
   && myNbSamples    == theNbSamples)
  {
    return IsValid();
  }

  return Init (theGlContext, theViewportSizeX, theViewportSizeY, theColorFormats, theDepthFormat, theNbSamples);
}

// =======================================================================
// function : InitWithRB
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::InitWithRB (const Handle(OpenGl_Context)& theGlCtx,
                                                 const GLsizei                 theSizeX,
                                                 const GLsizei                 theSizeY,
                                                 const GLint                   theColorFormat,
                                                 const GLint                   theDepthFormat,
                                                 const GLuint                  theColorRBufferFromWindow)
{
  myColorFormats.Clear();
  myColorFormats.Append (theColorFormat);
  if (!myColorTextures.IsEmpty())
  {
    Handle(OpenGl_Texture) aTexutre = myColorTextures.First();
    for (OpenGl_TextureArray::Iterator aTextureIt (myColorTextures); aTextureIt.More(); aTextureIt.Next())
    {
      aTextureIt.Value()->Release (theGlCtx.operator->());
    }
    myColorTextures.Clear();
    myColorTextures.Append (aTexutre);
  }

  myDepthFormat = theDepthFormat;
  myNbSamples   = 0;
  myInitVPSizeX = theSizeX;
  myInitVPSizeY = theSizeY;
  if (theGlCtx->arbFBO == NULL)
  {
    return Standard_False;
  }

  // clean up previous state
  Release (theGlCtx.operator->());

  myIsOwnBuffer = true;
  myIsOwnDepth  = true;

  // setup viewport sizes as is
  myVPSizeX = theSizeX;
  myVPSizeY = theSizeY;
  const Standard_Integer aSizeX = theSizeX > 0 ? theSizeX : 2;
  const Standard_Integer aSizeY = theSizeY > 0 ? theSizeY : 2;

  // Create the render-buffers
  if (theColorRBufferFromWindow != NO_RENDERBUFFER)
  {
    myGlColorRBufferId = theColorRBufferFromWindow;
  }
  else if (theColorFormat != 0)
  {
    theGlCtx->arbFBO->glGenRenderbuffers (1, &myGlColorRBufferId);
    theGlCtx->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, myGlColorRBufferId);
    theGlCtx->arbFBO->glRenderbufferStorage (GL_RENDERBUFFER, theColorFormat, aSizeX, aSizeY);
  }

  bool hasStencilRB = false;
  if (myDepthFormat != 0)
  {
    const OpenGl_TextureFormat aDepthFormat = OpenGl_TextureFormat::FindSizedFormat (theGlCtx, myDepthFormat);
    hasStencilRB = aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL;

    theGlCtx->arbFBO->glGenRenderbuffers (1, &myGlDepthRBufferId);
    theGlCtx->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, myGlDepthRBufferId);
    theGlCtx->arbFBO->glRenderbufferStorage (GL_RENDERBUFFER, myDepthFormat, aSizeX, aSizeY);
    theGlCtx->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, NO_RENDERBUFFER);
  }

  // create FBO
  theGlCtx->arbFBO->glGenFramebuffers (1, &myGlFBufferId);
  theGlCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, myGlFBufferId);
  theGlCtx->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                               GL_RENDERBUFFER, myGlColorRBufferId);
  if (myGlDepthRBufferId != NO_RENDERBUFFER)
  {
    if (hasDepthStencilAttach (theGlCtx) && hasStencilRB)
    {
      theGlCtx->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                                   GL_RENDERBUFFER, myGlDepthRBufferId);
    }
    else
    {
      theGlCtx->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                   GL_RENDERBUFFER, myGlDepthRBufferId);
      if (hasStencilRB)
      {
        theGlCtx->arbFBO->glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                                     GL_RENDERBUFFER, myGlDepthRBufferId);
      }
    }
  }
  if (theGlCtx->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    UnbindBuffer (theGlCtx);
    Release (theGlCtx.operator->());
    return Standard_False;
  }

  UnbindBuffer (theGlCtx);
  return Standard_True;
}

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::InitWrapper (const Handle(OpenGl_Context)& theGlCtx)
{
  myNbSamples = 0;
  if (theGlCtx->arbFBO == NULL)
  {
    return Standard_False;
  }

  // clean up previous state
  Release (theGlCtx.operator->());

  GLint anFbo = GLint(NO_FRAMEBUFFER);
  ::glGetIntegerv (GL_FRAMEBUFFER_BINDING, &anFbo);
  if (anFbo == GLint(NO_FRAMEBUFFER))
  {
    return Standard_False;
  }

  GLint aColorType = 0;
  GLint aColorId   = 0;
  GLint aDepthType = 0;
  GLint aDepthId   = 0;
  theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &aColorType);
  theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &aDepthType);

  myGlFBufferId = GLuint(anFbo);
  myIsOwnBuffer = false;
  myIsOwnDepth  = false;
  if (aColorType == GL_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &aColorId);
    myGlColorRBufferId = aColorId;
  }
  else if (aColorType != GL_NONE)
  {
    TCollection_ExtendedString aMsg = "OpenGl_FrameBuffer::InitWrapper(), color attachment of unsupported type has been skipped!";
    theGlCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_ERROR,
                           0,
                           GL_DEBUG_SEVERITY_HIGH,
                           aMsg);
  }

  if (aDepthType == GL_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glGetFramebufferAttachmentParameteriv (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &aDepthId);
    myGlDepthRBufferId = aDepthId;
  }
  else if (aDepthType != GL_NONE)
  {
    TCollection_ExtendedString aMsg = "OpenGl_FrameBuffer::InitWrapper(), depth attachment of unsupported type has been skipped!";
    theGlCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                           GL_DEBUG_TYPE_ERROR,
                           0,
                           GL_DEBUG_SEVERITY_HIGH,
                           aMsg);
  }

  // retrieve dimensions
  GLuint aRBuffer = myGlColorRBufferId != NO_RENDERBUFFER ? myGlColorRBufferId : myGlDepthRBufferId;
  if (aRBuffer != NO_RENDERBUFFER)
  {
    theGlCtx->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, aRBuffer);
    theGlCtx->arbFBO->glGetRenderbufferParameteriv (GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,  &myVPSizeX);
    theGlCtx->arbFBO->glGetRenderbufferParameteriv (GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &myVPSizeY);
    theGlCtx->arbFBO->glBindRenderbuffer (GL_RENDERBUFFER, NO_RENDERBUFFER);
  }

  return aRBuffer != NO_RENDERBUFFER;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::Release (OpenGl_Context* theGlCtx)
{
  if (isValidFrameBuffer())
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN (theGlCtx != NULL,
      "OpenGl_FrameBuffer destroyed without GL context! Possible GPU memory leakage...",);
    if (theGlCtx->IsValid()
     && myIsOwnBuffer)
    {
      theGlCtx->arbFBO->glDeleteFramebuffers (1, &myGlFBufferId);
      if (myGlColorRBufferId != NO_RENDERBUFFER)
      {
        theGlCtx->arbFBO->glDeleteRenderbuffers (1, &myGlColorRBufferId);
      }
      if (myGlDepthRBufferId != NO_RENDERBUFFER)
      {
        theGlCtx->arbFBO->glDeleteRenderbuffers (1, &myGlDepthRBufferId);
      }
    }
    myGlFBufferId      = NO_FRAMEBUFFER;
    myGlColorRBufferId = NO_RENDERBUFFER;
    myGlDepthRBufferId = NO_RENDERBUFFER;
    myIsOwnBuffer      = false;
  }

  for (Standard_Integer aColorBufferIdx = 0; aColorBufferIdx < myColorTextures.Length(); ++aColorBufferIdx)
  {
    myColorTextures (aColorBufferIdx)->Release (theGlCtx);
  }

  if (myIsOwnDepth)
  {
    myDepthStencilTexture->Release (theGlCtx);
    myIsOwnDepth = false;
  }

  myVPSizeX = 0;
  myVPSizeY = 0;
}

// =======================================================================
// function : SetupViewport
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::SetupViewport (const Handle(OpenGl_Context)& theGlCtx)
{
  const Standard_Integer aViewport[4] = { 0, 0, myVPSizeX, myVPSizeY };
  theGlCtx->ResizeViewport (aViewport);
}

// =======================================================================
// function : ChangeViewport
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::ChangeViewport (const GLsizei theVPSizeX,
                                         const GLsizei theVPSizeY)
{
  myVPSizeX = theVPSizeX;
  myVPSizeY = theVPSizeY;
}

// =======================================================================
// function : BindBuffer
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::BindBuffer (const Handle(OpenGl_Context)& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, myGlFBufferId);
  theGlCtx->SetFrameBufferSRGB (true);
}

// =======================================================================
// function : BindDrawBuffer
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::BindDrawBuffer (const Handle(OpenGl_Context)& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, myGlFBufferId);
  theGlCtx->SetFrameBufferSRGB (true);
}

// =======================================================================
// function : BindReadBuffer
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::BindReadBuffer (const Handle(OpenGl_Context)& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer (GL_READ_FRAMEBUFFER, myGlFBufferId);
}

// =======================================================================
// function : UnbindBuffer
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::UnbindBuffer (const Handle(OpenGl_Context)& theGlCtx)
{
  if (!theGlCtx->DefaultFrameBuffer().IsNull()
   &&  theGlCtx->DefaultFrameBuffer().operator->() != this)
  {
    theGlCtx->DefaultFrameBuffer()->BindBuffer (theGlCtx);
  }
  else
  {
    theGlCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, NO_FRAMEBUFFER);
    theGlCtx->SetFrameBufferSRGB (false);
  }
}

// =======================================================================
// function : getAligned
// purpose  :
// =======================================================================
inline Standard_Size getAligned (const Standard_Size theNumber,
                                 const Standard_Size theAlignment)
{
  return theNumber + theAlignment - 1 - (theNumber - 1) % theAlignment;
}

template<typename T>
inline void convertRowFromRgba (T* theRgbRow,
                                const Image_ColorRGBA* theRgbaRow,
                                const Standard_Size theWidth)
{
  for (Standard_Size aCol = 0; aCol < theWidth; ++aCol)
  {
    const Image_ColorRGBA& anRgba = theRgbaRow[aCol];
    T& anRgb = theRgbRow[aCol];
    anRgb.r() = anRgba.r();
    anRgb.g() = anRgba.g();
    anRgb.b() = anRgba.b();
  }
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::BufferDump (const Handle(OpenGl_Context)& theGlCtx,
                                                 const Handle(OpenGl_FrameBuffer)& theFbo,
                                                 Image_PixMap& theImage,
                                                 Graphic3d_BufferType theBufferType)
{
  if (theGlCtx.IsNull()
   || theImage.IsEmpty())
  {
    return Standard_False;
  }

  GLenum aFormat = 0;
  GLenum aType   = 0;
  bool toSwapRgbaBgra = false;
  bool toConvRgba2Rgb = false;
  switch (theImage.Format())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case Image_Format_Gray:
      aFormat = theBufferType == Graphic3d_BT_Depth ? GL_DEPTH_COMPONENT : GL_RED;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_GrayF:
      aFormat = theBufferType == Graphic3d_BT_Depth ? GL_DEPTH_COMPONENT : GL_RED;
      aType   = GL_FLOAT;
      break;
    case Image_Format_RGF:
      aFormat = GL_RG;
      aType   = GL_FLOAT;
      break;
    case Image_Format_RGB:
      aFormat = GL_RGB;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_BGR:
      aFormat = GL_BGR;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_BGRA:
    case Image_Format_BGR32:
      aFormat = GL_BGRA;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_BGRF:
      aFormat = GL_BGR;
      aType   = GL_FLOAT;
      break;
    case Image_Format_BGRAF:
      aFormat = GL_BGRA;
      aType   = GL_FLOAT;
      break;
  #else
    case Image_Format_Gray:
    case Image_Format_GrayF:
    case Image_Format_BGRF:
    case Image_Format_BGRAF:
    case Image_Format_RGF:
      return Standard_False;
    case Image_Format_BGRA:
    case Image_Format_BGR32:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      toSwapRgbaBgra = true;
      break;
    case Image_Format_BGR:
    case Image_Format_RGB:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      toConvRgba2Rgb = true;
      break;
  #endif
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
    case Image_Format_Alpha:
    case Image_Format_AlphaF:
      return Standard_False; // GL_ALPHA is no more supported in core context
    case Image_Format_UNKNOWN:
      return Standard_False;
  }

  if (aFormat == 0)
  {
    return Standard_False;
  }

#if !defined(GL_ES_VERSION_2_0)
  GLint aReadBufferPrev = GL_BACK;
  if (theBufferType == Graphic3d_BT_Depth
   && aFormat != GL_DEPTH_COMPONENT)
  {
    return Standard_False;
  }
#else
  (void )theBufferType;
#endif

  // bind FBO if used
  if (!theFbo.IsNull() && theFbo->IsValid())
  {
    theFbo->BindBuffer (theGlCtx);
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    glGetIntegerv (GL_READ_BUFFER, &aReadBufferPrev);
    GLint aDrawBufferPrev = GL_BACK;
    glGetIntegerv (GL_DRAW_BUFFER, &aDrawBufferPrev);
    glReadBuffer (aDrawBufferPrev);
  #endif
  }

  // setup alignment
  const GLint anAligment   = Min (GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL
  glPixelStorei (GL_PACK_ALIGNMENT, anAligment);
  bool isBatchCopy = !theImage.IsTopDown();

  const GLint   anExtraBytes       = GLint(theImage.RowExtraBytes());
  GLint         aPixelsWidth       = GLint(theImage.SizeRowBytes() / theImage.SizePixelBytes());
  Standard_Size aSizeRowBytesEstim = getAligned (theImage.SizePixelBytes() * aPixelsWidth, anAligment);
  if (anExtraBytes < anAligment)
  {
    aPixelsWidth = 0;
  }
  else if (aSizeRowBytesEstim != theImage.SizeRowBytes())
  {
    aPixelsWidth = 0;
    isBatchCopy  = false;
  }
#if !defined(GL_ES_VERSION_2_0)
  glPixelStorei (GL_PACK_ROW_LENGTH, aPixelsWidth);
#else
  if (aPixelsWidth != 0)
  {
    isBatchCopy = false;
  }
#endif
  if (toConvRgba2Rgb)
  {
    Handle(NCollection_BaseAllocator) anAlloc = new NCollection_AlignedAllocator (16);
    const Standard_Size aRowSize = theImage.SizeX() * 4;
    NCollection_Buffer aRowBuffer (anAlloc);
    if (!aRowBuffer.Allocate (aRowSize))
    {
      return Standard_False;
    }

    for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      // Image_PixMap rows indexation always starts from the upper corner
      // while order in memory depends on the flag and processed by ChangeRow() method
      glReadPixels (0, GLint(theImage.SizeY() - aRow - 1), GLsizei (theImage.SizeX()), 1, aFormat, aType, aRowBuffer.ChangeData());
      const Image_ColorRGBA* aRowDataRgba = (const Image_ColorRGBA* )aRowBuffer.Data();
      if (theImage.Format() == Image_Format_BGR)
      {
        convertRowFromRgba ((Image_ColorBGR* )theImage.ChangeRow (aRow), aRowDataRgba, theImage.SizeX());
      }
      else
      {
        convertRowFromRgba ((Image_ColorRGB* )theImage.ChangeRow (aRow), aRowDataRgba, theImage.SizeX());
      }
    }
  }
  else if (!isBatchCopy)
  {
    // copy row by row
    for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      // Image_PixMap rows indexation always starts from the upper corner
      // while order in memory depends on the flag and processed by ChangeRow() method
      glReadPixels (0, GLint(theImage.SizeY() - aRow - 1), GLsizei (theImage.SizeX()), 1, aFormat, aType, theImage.ChangeRow (aRow));
    }
  }
  else
  {
    glReadPixels (0, 0, GLsizei (theImage.SizeX()), GLsizei (theImage.SizeY()), aFormat, aType, theImage.ChangeData());
  }
  const bool hasErrors = theGlCtx->ResetErrors (true);

  glPixelStorei (GL_PACK_ALIGNMENT,  1);
#if !defined(GL_ES_VERSION_2_0)
  glPixelStorei (GL_PACK_ROW_LENGTH, 0);
#endif

  if (!theFbo.IsNull() && theFbo->IsValid())
  {
    theFbo->UnbindBuffer (theGlCtx);
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    glReadBuffer (aReadBufferPrev);
  #endif
  }

  if (toSwapRgbaBgra)
  {
    Image_PixMap::SwapRgbaBgra (theImage);
  }

  return !hasErrors;
}

// =======================================================================
// function : EstimatedDataSize
// purpose  :
// =======================================================================
Standard_Size OpenGl_FrameBuffer::EstimatedDataSize() const
{
  if (!IsValid())
  {
    return 0;
  }

  Standard_Size aSize = 0;
  for (OpenGl_TextureArray::Iterator aTextureIt (myColorTextures); aTextureIt.More(); aTextureIt.Next())
  {
    aSize += aTextureIt.Value()->EstimatedDataSize();
  }
  if (!myDepthStencilTexture.IsNull())
  {
    aSize += myDepthStencilTexture->EstimatedDataSize();
  }
  if (myGlColorRBufferId != NO_RENDERBUFFER
  && !myColorFormats.IsEmpty())
  {
    aSize += OpenGl_Texture::PixelSizeOfPixelFormat (myColorFormats.First()) * myInitVPSizeX * myInitVPSizeY;
  }
  if (myGlDepthRBufferId != NO_RENDERBUFFER)
  {
    aSize += OpenGl_Texture::PixelSizeOfPixelFormat (myDepthFormat) * myInitVPSizeX * myInitVPSizeY;
  }
  return aSize;
}
