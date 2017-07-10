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

#include <OpenGl_TextureBufferArb.hxx>

#include <OpenGl_Context.hxx>
#include <Standard_Assert.hxx>


IMPLEMENT_STANDARD_RTTIEXT(OpenGl_TextureBufferArb,OpenGl_VertexBuffer)

// =======================================================================
// function : OpenGl_TextureBufferArb
// purpose  :
// =======================================================================
OpenGl_TextureBufferArb::OpenGl_TextureBufferArb()
: OpenGl_VertexBuffer(),
  myTextureId (NO_TEXTURE),
  myTexFormat (GL_RGBA32F)
{
  //
}

// =======================================================================
// function : ~OpenGl_TextureBufferArb
// purpose  :
// =======================================================================
OpenGl_TextureBufferArb::~OpenGl_TextureBufferArb()
{
  Release (NULL);
}

// =======================================================================
// function : GetTarget
// purpose  :
// =======================================================================
GLenum OpenGl_TextureBufferArb::GetTarget() const
{
  return GL_TEXTURE_BUFFER; // GL_TEXTURE_BUFFER for OpenGL 3.1+, OpenGL ES 3.2
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_TextureBufferArb::Release (OpenGl_Context* theGlCtx)
{
  if (myTextureId != NO_TEXTURE)
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN (theGlCtx != NULL,
      "OpenGl_TextureBufferExt destroyed without GL context! Possible GPU memory leakage...",);

    if (theGlCtx->IsValid())
    {
      glDeleteTextures (1, &myTextureId);
    }
    myTextureId = NO_TEXTURE;
  }
  OpenGl_VertexBuffer::Release (theGlCtx);
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_TextureBufferArb::Create (const Handle(OpenGl_Context)& theGlCtx)
{
  if (!OpenGl_VertexBuffer::Create (theGlCtx))
  {
    return false;
  }

  if (myTextureId == NO_TEXTURE)
  {
    glGenTextures (1, &myTextureId);
  }
  return myTextureId != NO_TEXTURE;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBufferArb::Init (const Handle(OpenGl_Context)& theGlCtx,
                                    const GLuint   theComponentsNb,
                                    const GLsizei  theElemsNb,
                                    const GLfloat* theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1
        || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (theComponentsNb == 3
       && !theGlCtx->arbTboRGB32)
  {
    return false;
  }
  else if (!Create (theGlCtx)
        || !OpenGl_VertexBuffer::Init (theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1: myTexFormat = GL_R32F;    break;
    case 2: myTexFormat = GL_RG32F;   break;
    case 3: myTexFormat = GL_RGB32F;  break; // GL_ARB_texture_buffer_object_rgb32
    case 4: myTexFormat = GL_RGBA32F; break;
  }

  Bind (theGlCtx);
  BindTexture  (theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer (GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind (theGlCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBufferArb::Init (const Handle(OpenGl_Context)& theGlCtx,
                                    const GLuint   theComponentsNb,
                                    const GLsizei  theElemsNb,
                                    const GLuint*  theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1
        || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (theComponentsNb == 3
       && !theGlCtx->arbTboRGB32)
  {
    return false;
  }
  else if (!Create (theGlCtx)
        || !OpenGl_VertexBuffer::Init (theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1: myTexFormat = GL_R32I;    break;
    case 2: myTexFormat = GL_RG32I;   break;
    case 3: myTexFormat = GL_RGB32I;  break;
    case 4: myTexFormat = GL_RGBA32I; break;
  }

  Bind (theGlCtx);
  BindTexture  (theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer (GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind (theGlCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBufferArb::Init (const Handle(OpenGl_Context)& theGlCtx,
                                    const GLuint theComponentsNb,
                                    const GLsizei theElemsNb,
                                    const GLushort* theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1
        || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (!Create (theGlCtx)
        || !OpenGl_VertexBuffer::Init (theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1: myTexFormat = GL_R16I;    break;
    case 2: myTexFormat = GL_RG16I;   break;
    case 3: myTexFormat = GL_RGB16I;  break;
    case 4: myTexFormat = GL_RGBA16I; break;
  }

  Bind (theGlCtx);
  BindTexture  (theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer (GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind (theGlCtx);
  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_TextureBufferArb::Init (const Handle(OpenGl_Context)& theGlCtx,
                                    const GLuint   theComponentsNb,
                                    const GLsizei  theElemsNb,
                                    const GLubyte*  theData)
{
  if (theGlCtx->arbTBO == NULL)
  {
    return false;
  }
  else if (theComponentsNb < 1
        || theComponentsNb > 4)
  {
    // unsupported format
    return false;
  }
  else if (!Create (theGlCtx)
        || !OpenGl_VertexBuffer::Init (theGlCtx, theComponentsNb, theElemsNb, theData))
  {
    return false;
  }

  switch (theComponentsNb)
  {
    case 1: myTexFormat = GL_R8;    break;
    case 2: myTexFormat = GL_RG8;   break;
    case 3: myTexFormat = GL_RGB8;  break;
    case 4: myTexFormat = GL_RGBA8; break;
  }

  Bind (theGlCtx);
  BindTexture  (theGlCtx, Graphic3d_TextureUnit_0);
  theGlCtx->arbTBO->glTexBuffer (GetTarget(), myTexFormat, myBufferId);
  UnbindTexture(theGlCtx, Graphic3d_TextureUnit_0);
  Unbind (theGlCtx);
  return true;
}

// =======================================================================
// function : BindTexture
// purpose  :
// =======================================================================
void OpenGl_TextureBufferArb::BindTexture (const Handle(OpenGl_Context)& theGlCtx,
                                           const Graphic3d_TextureUnit   theTextureUnit) const
{
  theGlCtx->core20fwd->glActiveTexture (GL_TEXTURE0 + theTextureUnit);
  glBindTexture (GetTarget(), myTextureId);
}

// =======================================================================
// function : UnbindTexture
// purpose  :
// =======================================================================
void OpenGl_TextureBufferArb::UnbindTexture (const Handle(OpenGl_Context)& theGlCtx,
                                             const Graphic3d_TextureUnit   theTextureUnit) const
{
  theGlCtx->core20fwd->glActiveTexture (GL_TEXTURE0 + theTextureUnit);
  glBindTexture (GetTarget(), NO_TEXTURE);
}
