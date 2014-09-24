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

#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_FrameBuffer, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_FrameBuffer, OpenGl_Resource)

static inline bool isOddNumber (const GLsizei theNumber)
{
  return theNumber & 0x01;
}

static inline GLsizei getEvenNumber (const GLsizei theNumber)
{
  return isOddNumber (theNumber) ? (theNumber + 1) : theNumber;
}

//! Notice - 0 is not power of two here
static inline bool isPowerOfTwo (const GLsizei theNumber)
{
  return !(theNumber & (theNumber - 1));
}

// =======================================================================
// function : OpenGl_FrameBuffer
// purpose  :
// =======================================================================
OpenGl_FrameBuffer::OpenGl_FrameBuffer (GLint theTextureFormat)
: myVPSizeX (0),
  myVPSizeY (0),
  myTextFormat (theTextureFormat),
  myGlFBufferId (NO_FRAMEBUFFER),
  myColorTexture (new OpenGl_Texture()),
  myDepthStencilTexture (new OpenGl_Texture())
{
  //
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
                                           const GLsizei   theViewportSizeX,
                                           const GLsizei   theViewportSizeY)
{
  if (theGlContext->arbFBO == NULL)
  {
    return Standard_False;
  }

  // clean up previous state
  Release (theGlContext.operator->());

  // setup viewport sizes as is
  myVPSizeX = theViewportSizeX;
  myVPSizeY = theViewportSizeY;

  // Create the textures (will be used as color buffer and depth-stencil buffer)
  if (!initTrashTextures (theGlContext))
  {
    Release (theGlContext.operator->());
    return Standard_False;
  }

  // Build FBO and setup it as texture
  theGlContext->arbFBO->glGenFramebuffers (1, &myGlFBufferId);
  theGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, myGlFBufferId);
  theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                GL_TEXTURE_2D, myColorTexture->TextureId(), 0);
#ifdef GL_DEPTH_STENCIL_ATTACHMENT
  theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                                GL_TEXTURE_2D, myDepthStencilTexture->TextureId(), 0);
#else
  theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                GL_TEXTURE_2D, myDepthStencilTexture->TextureId(), 0);
  theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                                GL_TEXTURE_2D, myDepthStencilTexture->TextureId(), 0);
#endif
  if (theGlContext->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Release (theGlContext.operator->());
    return Standard_False;
  }

  UnbindBuffer (theGlContext);
  return Standard_True;
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
    if (theGlCtx->IsValid())
    {
      theGlCtx->arbFBO->glDeleteFramebuffers (1, &myGlFBufferId);
    }
    myGlFBufferId = NO_FRAMEBUFFER;
  }

  myColorTexture->Release (theGlCtx);
  myDepthStencilTexture->Release (theGlCtx);
}

// =======================================================================
// function : initTrashTexture
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::initTrashTextures (const Handle(OpenGl_Context)& theGlContext)
{
  return    myColorTexture->Init (theGlContext, myTextFormat,
                                  GL_RGBA, GL_UNSIGNED_BYTE,
                                  myVPSizeX, myVPSizeY, Graphic3d_TOT_2D)
  && myDepthStencilTexture->Init (theGlContext, GL_DEPTH24_STENCIL8,
                                  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
                                  myVPSizeX, myVPSizeY, Graphic3d_TOT_2D);
}

// =======================================================================
// function : SetupViewport
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::SetupViewport (const Handle(OpenGl_Context)& /*theGlCtx*/)
{
  glViewport (0, 0, myVPSizeX, myVPSizeY);
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
}

// =======================================================================
// function : UnbindBuffer
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::UnbindBuffer (const Handle(OpenGl_Context)& theGlCtx)
{
  theGlCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, NO_FRAMEBUFFER);
}
