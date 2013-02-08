// Created by: Kirill GAVRILOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_FrameBuffer.hxx>

#ifdef DEB
  #include <iostream>
#endif

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

OpenGl_FrameBuffer::OpenGl_FrameBuffer (GLint theTextureFormat)
: mySizeX (0),
  mySizeY (0),
  myVPSizeX (0),
  myVPSizeY (0),
  myTextFormat (theTextureFormat),
  myGlTextureId (NO_TEXTURE),
  myGlFBufferId (NO_FRAMEBUFFER),
  myGlDepthRBId (NO_RENDERBUFFER)
{
  //
}

Standard_Boolean OpenGl_FrameBuffer::Init (const Handle(OpenGl_Context)& theGlContext,
                                           GLsizei theViewportSizeX,
                                           GLsizei theViewportSizeY,
                                           GLboolean toForcePowerOfTwo)
{
  if (theGlContext->extFBO == NULL)
  {
  #ifdef DEB
    std::cerr << "OpenGl_FrameBuffer, FBO extension not supported!\n";
  #endif
    return Standard_False;
  }

  // clean up previous state
  Release (theGlContext);

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
  if (!InitTrashTexture (theGlContext))
  {
    if (!isPowerOfTwo (mySizeX) || !isPowerOfTwo (mySizeY))
    {
      return Init (theGlContext, theViewportSizeX, theViewportSizeY, GL_TRUE);
    }
    Release (theGlContext);
    return Standard_False;
  }

  // Create RenderBuffer (will be used as depth buffer)
  theGlContext->extFBO->glGenRenderbuffersEXT (1, &myGlDepthRBId);
  theGlContext->extFBO->glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, myGlDepthRBId);
  theGlContext->extFBO->glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mySizeX, mySizeY);

  // Build FBO and setup it as texture
  theGlContext->extFBO->glGenFramebuffersEXT (1, &myGlFBufferId);
  theGlContext->extFBO->glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, myGlFBufferId);
  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);
  theGlContext->extFBO->glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, myGlTextureId, 0);
  theGlContext->extFBO->glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, myGlFBufferId);
  if (theGlContext->extFBO->glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    if (!isPowerOfTwo (mySizeX) || !isPowerOfTwo (mySizeY))
    {
      return Init (theGlContext, theViewportSizeX, theViewportSizeY, GL_TRUE);
    }
    Release (theGlContext);
    return Standard_False;
  }

  UnbindBuffer (theGlContext);
  UnbindTexture();
  theGlContext->extFBO->glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, NO_RENDERBUFFER);

  #ifdef DEB
    std::cerr << "OpenGl_FrameBuffer, created FBO " << mySizeX << "x" << mySizeY
              << " for viewport " << theViewportSizeX << "x" << theViewportSizeY << "\n";
  #endif
  return Standard_True;
}

void OpenGl_FrameBuffer::Release (const Handle(OpenGl_Context)& theGlContext)
{
  if (IsValidDepthBuffer())
  {
    if (!theGlContext.IsNull() && theGlContext->extFBO != NULL)
    {
      theGlContext->extFBO->glDeleteRenderbuffersEXT (1, &myGlDepthRBId);
      myGlDepthRBId = NO_RENDERBUFFER;
    }
    else
    {
      std::cerr << "OpenGl_FrameBuffer::Release() called with invalid OpenGl_Context!\n";
    }
  }
  if (IsValidTexture())
  {
    glDeleteTextures (1, &myGlTextureId);
    myGlTextureId = NO_TEXTURE;
  }
  mySizeX = mySizeY = myVPSizeX = myVPSizeY = 0;
  if (IsValidFrameBuffer())
  {
    if (!theGlContext.IsNull() && theGlContext->extFBO != NULL)
    {
      theGlContext->extFBO->glDeleteFramebuffersEXT (1, &myGlFBufferId);
      myGlFBufferId = NO_FRAMEBUFFER;
    }
    else
    {
      std::cerr << "OpenGl_FrameBuffer::Release() called with invalid OpenGl_Context!\n";
    }
  }
}

Standard_Boolean OpenGl_FrameBuffer::IsProxySuccess() const
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

Standard_Boolean OpenGl_FrameBuffer::InitTrashTexture (const Handle(OpenGl_Context)& theGlContext)
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
  if (!IsValidTexture())
  {
    glGenTextures (1, &myGlTextureId); // Create The Texture
  }
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);

  // texture interpolation parameters - could be overridden later
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (!IsProxySuccess())
  {
    Release (theGlContext);
    return Standard_False;
  }

  glTexImage2D (GL_TEXTURE_2D,
                0,                // LOD number: 0 - base image level; n is the nth mipmap reduction image
                myTextFormat,     // internalformat
                mySizeX, mySizeY, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL); // NULL pointer supported from OpenGL 1.1
  return Standard_True;
}
