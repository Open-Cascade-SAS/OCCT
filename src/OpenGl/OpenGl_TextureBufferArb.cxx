// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
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

#include <OpenGl_TextureBufferArb.hxx>

#include <OpenGl_Context.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_TextureBufferArb, OpenGl_VertexBuffer)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_TextureBufferArb, OpenGl_VertexBuffer)

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
  return GL_TEXTURE_BUFFER_ARB; // GL_TEXTURE_BUFFER for OpenGL 3.1+
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_TextureBufferArb::Release (const OpenGl_Context* theGlCtx)
{
  if (myTextureId != NO_TEXTURE)
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN (theGlCtx != NULL,
      "OpenGl_TextureBufferExt destroyed without GL context! Possible GPU memory leakage...",);

    glDeleteTextures (1, &myTextureId);
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
  if (theComponentsNb != 1
   && theComponentsNb != 2
   && theComponentsNb != 4)
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
    case 1: myTexFormat = GL_R32F;    break;
    case 2: myTexFormat = GL_RG32F;   break;
    //case 3: myTexFormat = GL_RGB32F;  break; // GL_ARB_texture_buffer_object_rgb32
    case 4: myTexFormat = GL_RGBA32F; break;
  }

  Bind (theGlCtx);
  BindTexture (theGlCtx);
  theGlCtx->arbTBO->glTexBufferARB (GetTarget(), myTexFormat, myBufferId);
  UnbindTexture (theGlCtx);
  Unbind (theGlCtx);
  return true;
}

// =======================================================================
// function : BindTexture
// purpose  :
// =======================================================================
void OpenGl_TextureBufferArb::BindTexture (const Handle(OpenGl_Context)& theGlCtx,
                                           const GLenum theTextureUnit) const
{
  theGlCtx->core20->glActiveTexture (theTextureUnit);
  glBindTexture (GetTarget(), myTextureId);
}

// =======================================================================
// function : UnbindTexture
// purpose  :
// =======================================================================
void OpenGl_TextureBufferArb::UnbindTexture (const Handle(OpenGl_Context)& theGlCtx,
                                             const GLenum theTextureUnit) const
{
  theGlCtx->core20->glActiveTexture (theTextureUnit);
  glBindTexture (GetTarget(), NO_TEXTURE);
}
