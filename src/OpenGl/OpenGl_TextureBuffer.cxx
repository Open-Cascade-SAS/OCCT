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

#include <OpenGl_TextureBuffer.hxx>

#include <OpenGl_ArbTBO.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_Context.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_TextureBuffer, OpenGl_Buffer)

// =======================================================================
// function : OpenGl_TextureBuffer
// purpose  :
// =======================================================================
OpenGl_TextureBuffer::OpenGl_TextureBuffer()
    : OpenGl_Buffer(),
      myTextureId(NO_TEXTURE),
      myTexFormat(GL_RGBA32F)
{
  //
}

// =======================================================================
// function : ~OpenGl_TextureBuffer
// purpose  :
// =======================================================================
OpenGl_TextureBuffer::~OpenGl_TextureBuffer()
{
  Release(NULL);
}

// =======================================================================
// function : GetTarget
// purpose  :
// =======================================================================
unsigned int OpenGl_TextureBuffer::GetTarget() const
{
  return GL_TEXTURE_BUFFER; // GL_TEXTURE_BUFFER for OpenGL 3.1+, OpenGL ES 3.2
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_TextureBuffer::Release(OpenGl_Context* theGlCtx)
{
  if (myTextureId != NO_TEXTURE)
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN(
      theGlCtx != NULL,
      "OpenGl_TextureBuffer destroyed without GL context! Possible GPU memory leakage...", );

    if (theGlCtx->IsValid())
    {
      theGlCtx->core20fwd->glDeleteTextures(1, &myTextureId);
    }
    myTextureId = NO_TEXTURE;
  }
  base_type::Release(theGlCtx);
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_TextureBuffer::Create(const Handle(OpenGl_Context)& theGlCtx)
{
  if (!base_type::Create(theGlCtx))
  {
    return false;
  }

  if (myTextureId == NO_TEXTURE)
  {
    theGlCtx->core20fwd->glGenTextures(1, &myTextureId);
  }
  return myTextureId != NO_TEXTURE;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBuffer::Init(const Handle(OpenGl_Context)& theGlCtx,
                                const unsigned int            theComponentsNb,
                                const Standard_Integer        theElemsNb,
                                const float*                  theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1 || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (theComponentsNb == 3 && !theGlCtx->arbTboRGB32)
  {
    return false;
  }
  else if (!Create(theGlCtx) || !base_type::Init(theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1:
      myTexFormat = GL_R32F;
      break;
    case 2:
      myTexFormat = GL_RG32F;
      break;
    case 3:
      myTexFormat = GL_RGB32F;
      break; // GL_ARB_texture_buffer_object_rgb32
    case 4:
      myTexFormat = GL_RGBA32F;
      break;
  }

  Bind(theGlCtx);
  BindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer(GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind(theGlCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBuffer::Init(const Handle(OpenGl_Context)& theGlCtx,
                                const unsigned int            theComponentsNb,
                                const Standard_Integer        theElemsNb,
                                const unsigned int*           theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1 || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (theComponentsNb == 3 && !theGlCtx->arbTboRGB32)
  {
    return false;
  }
  else if (!Create(theGlCtx) || !base_type::Init(theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1:
      myTexFormat = GL_R32I;
      break;
    case 2:
      myTexFormat = GL_RG32I;
      break;
    case 3:
      myTexFormat = GL_RGB32I;
      break;
    case 4:
      myTexFormat = GL_RGBA32I;
      break;
  }

  Bind(theGlCtx);
  BindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer(GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind(theGlCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBuffer::Init(const Handle(OpenGl_Context)& theGlCtx,
                                const unsigned int            theComponentsNb,
                                const Standard_Integer        theElemsNb,
                                const unsigned short*         theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1 || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (!Create(theGlCtx) || !base_type::Init(theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1:
      myTexFormat = GL_R16I;
      break;
    case 2:
      myTexFormat = GL_RG16I;
      break;
    case 3:
      myTexFormat = GL_RGB16I;
      break;
    case 4:
      myTexFormat = GL_RGBA16I;
      break;
  }

  Bind(theGlCtx);
  BindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer(GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind(theGlCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBuffer::Init(const Handle(OpenGl_Context)& theGlCtx,
                                const unsigned int            theComponentsNb,
                                const Standard_Integer        theElemsNb,
                                const Standard_Byte*          theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1 || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (!Create(theGlCtx) || !base_type::Init(theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1:
      myTexFormat = GL_R8;
      break;
    case 2:
      myTexFormat = GL_RG8;
      break;
    case 3:
      myTexFormat = GL_RGB8;
      break;
    case 4:
      myTexFormat = GL_RGBA8;
      break;
  }

  Bind(theGlCtx);
  BindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer(GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind(theGlCtx);
  return true;
}

// =======================================================================
// function : BindTexture
// purpose  :
// =======================================================================
void OpenGl_TextureBuffer::BindTexture(const Handle(OpenGl_Context)& theGlCtx,
                                       const Graphic3d_TextureUnit   theTextureUnit) const
{
  theGlCtx->core20fwd->glActiveTexture(GL_TEXTURE0 + theTextureUnit);
  theGlCtx->core20fwd->glBindTexture(GetTarget(), myTextureId);
}

// =======================================================================
// function : UnbindTexture
// purpose  :
// =======================================================================
void OpenGl_TextureBuffer::UnbindTexture(const Handle(OpenGl_Context)& theGlCtx,
                                         const Graphic3d_TextureUnit   theTextureUnit) const
{
  theGlCtx->core20fwd->glActiveTexture(GL_TEXTURE0 + theTextureUnit);
  theGlCtx->core20fwd->glBindTexture(GetTarget(), NO_TEXTURE);
}
