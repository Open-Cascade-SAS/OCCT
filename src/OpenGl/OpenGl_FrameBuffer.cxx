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
: mySizeX (0),
  mySizeY (0),
  myVPSizeX (0),
  myVPSizeY (0),
  myTextFormat (theTextureFormat),
  myGlTextureId (NO_TEXTURE),
  myGlFBufferId (NO_FRAMEBUFFER),
  myGlDepthRBId (NO_RENDERBUFFER),
  myGlStencilRBId (NO_RENDERBUFFER)
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
                                           const GLsizei   theViewportSizeY,
                                           const GLboolean toForcePowerOfTwo)
{
  if (theGlContext->extFBO == NULL)
  {
    return Standard_False;
  }

  // clean up previous state
  Release (theGlContext.operator->());

  // upscale width/height if numbers are odd
  if (toForcePowerOfTwo)
  {
    mySizeX = OpenGl_Context::GetPowerOfTwo (theViewportSizeX, theGlContext->MaxTextureSize());
    mySizeY = OpenGl_Context::GetPowerOfTwo (theViewportSizeY, theGlContext->MaxTextureSize());
  }
  else
  {
    mySizeX = getEvenNumber (theViewportSizeX);
    mySizeY = getEvenNumber (theViewportSizeY);
  }

  // setup viewport sizes as is
  myVPSizeX = theViewportSizeX;
  myVPSizeY = theViewportSizeY;

  // Create the texture (will be used as color buffer)
  if (!initTrashTexture (theGlContext))
  {
    if (!isPowerOfTwo (mySizeX) || !isPowerOfTwo (mySizeY))
    {
      return Init (theGlContext, theViewportSizeX, theViewportSizeY, GL_TRUE);
    }
    Release (theGlContext.operator->());
    return Standard_False;
  }

  if (!theGlContext->extPDS)
  {
    // Create RenderBuffer to be used as depth buffer
    theGlContext->extFBO->glGenRenderbuffersEXT (1, &myGlDepthRBId);
    theGlContext->extFBO->glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, myGlDepthRBId);
    theGlContext->extFBO->glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mySizeX, mySizeY);

    // Create RenderBuffer to be used as stencil buffer
    theGlContext->extFBO->glGenRenderbuffersEXT (1, &myGlStencilRBId);
    theGlContext->extFBO->glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, myGlStencilRBId);
    theGlContext->extFBO->glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, mySizeX, mySizeY);
  }
  else
  {
    // Create combined depth stencil buffer
    theGlContext->extFBO->glGenRenderbuffersEXT (1, &myGlDepthRBId);
    theGlContext->extFBO->glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, myGlDepthRBId);
    theGlContext->extFBO->glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, mySizeX, mySizeY);
    myGlStencilRBId = myGlDepthRBId;
  }

  // Build FBO and setup it as texture
  theGlContext->extFBO->glGenFramebuffersEXT (1, &myGlFBufferId);
  theGlContext->extFBO->glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, myGlFBufferId);
  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);
  theGlContext->extFBO->glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, myGlTextureId, 0);
  theGlContext->extFBO->glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, myGlDepthRBId);
  theGlContext->extFBO->glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, myGlStencilRBId);
  if (theGlContext->extFBO->glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    if (!isPowerOfTwo (mySizeX) || !isPowerOfTwo (mySizeY))
    {
      return Init (theGlContext, theViewportSizeX, theViewportSizeY, GL_TRUE);
    }
    Release (theGlContext.operator->());
    return Standard_False;
  }

  UnbindBuffer  (theGlContext);
  UnbindTexture (theGlContext);
  theGlContext->extFBO->glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, NO_RENDERBUFFER);
  return Standard_True;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::Release (const OpenGl_Context* theGlCtx)
{
  if (isValidDepthBuffer()
   || isValidStencilBuffer()
   || isValidTexture()
   || isValidFrameBuffer())
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN (theGlCtx != NULL,
      "OpenGl_FrameBuffer destroyed without GL context! Possible GPU memory leakage...",);
  }
  if (isValidStencilBuffer())
  {
    if (theGlCtx->IsValid()
     && myGlStencilRBId != myGlDepthRBId)
    {
      theGlCtx->extFBO->glDeleteRenderbuffersEXT (1, &myGlStencilRBId);
    }
    myGlStencilRBId = NO_RENDERBUFFER;
  }
  if (isValidDepthBuffer())
  {
    if (theGlCtx->IsValid())
    {
      theGlCtx->extFBO->glDeleteRenderbuffersEXT (1, &myGlDepthRBId);
    }
    myGlDepthRBId = NO_RENDERBUFFER;
  }
  if (isValidTexture())
  {
    if (theGlCtx->IsValid())
    {
      glDeleteTextures (1, &myGlTextureId);
    }
    myGlTextureId = NO_TEXTURE;
  }
  mySizeX = mySizeY = myVPSizeX = myVPSizeY = 0;
  if (isValidFrameBuffer())
  {
    if (theGlCtx->IsValid())
    {
      theGlCtx->extFBO->glDeleteFramebuffersEXT (1, &myGlFBufferId);
    }
    myGlFBufferId = NO_FRAMEBUFFER;
  }
}

// =======================================================================
// function : isProxySuccess
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::isProxySuccess() const
{
  // use proxy to check texture could be created or not
  glTexImage2D (GL_PROXY_TEXTURE_2D,
                0,                // LOD number: 0 - base image level; n is the nth mipmap reduction image
                myTextFormat,     // internalformat
                mySizeX, mySizeY, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  GLint aTestParamX (0), aTestParamY (0);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &aTestParamX);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &aTestParamY);
  return aTestParamX != 0 && aTestParamY != 0;
}

// =======================================================================
// function : initTrashTexture
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_FrameBuffer::initTrashTexture (const Handle(OpenGl_Context)& theGlContext)
{
  // Check texture size is fit dimension maximum
  GLint aMaxTexDim = 2048;
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &aMaxTexDim);
  if (mySizeX > aMaxTexDim || mySizeY > aMaxTexDim)
  {
    return Standard_False;
  }

  // generate new id
  glEnable (GL_TEXTURE_2D);
  if (!isValidTexture())
  {
    glGenTextures (1, &myGlTextureId); // Create The Texture
  }
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);

  // texture interpolation parameters - could be overridden later
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (!isProxySuccess())
  {
    Release (theGlContext.operator->());
    return Standard_False;
  }

  glTexImage2D (GL_TEXTURE_2D,
                0,                // LOD number: 0 - base image level; n is the nth mipmap reduction image
                myTextFormat,     // internalformat
                mySizeX, mySizeY, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL); // NULL pointer supported from OpenGL 1.1
  return Standard_True;
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
  theGlCtx->extFBO->glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, myGlFBufferId);
}

// =======================================================================
// function : UnbindBuffer
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::UnbindBuffer (const Handle(OpenGl_Context)& theGlCtx)
{
  theGlCtx->extFBO->glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, NO_FRAMEBUFFER);
}

// =======================================================================
// function : BindTexture
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::BindTexture (const Handle(OpenGl_Context)& /*theGlCtx*/)
{
  glEnable (GL_TEXTURE_2D); // needed only for fixed pipeline rendering
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);
}

// =======================================================================
// function : UnbindTexture
// purpose  :
// =======================================================================
void OpenGl_FrameBuffer::UnbindTexture (const Handle(OpenGl_Context)& /*theGlCtx*/)
{
  glBindTexture (GL_TEXTURE_2D, NO_TEXTURE);
  glDisable (GL_TEXTURE_2D); // needed only for fixed pipeline rendering
}
