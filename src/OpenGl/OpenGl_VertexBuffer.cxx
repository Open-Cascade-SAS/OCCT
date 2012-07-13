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

#include <OpenGl_VertexBuffer.hxx>

#include <OpenGl_Context.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_VertexBuffer, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_VertexBuffer, OpenGl_Resource)

// =======================================================================
// function : OpenGl_VertexBuffer
// purpose  :
// =======================================================================
OpenGl_VertexBuffer::OpenGl_VertexBuffer()
: OpenGl_Resource(),
  myBufferId (NO_BUFFER),
  myComponentsNb (4),
  myElemsNb (0),
  myDataType (GL_FLOAT)
{
  //
}

// =======================================================================
// function : ~OpenGl_VertexBuffer
// purpose  :
// =======================================================================
OpenGl_VertexBuffer::~OpenGl_VertexBuffer()
{
  Release (NULL);
}

// =======================================================================
// function : GetTarget
// purpose  :
// =======================================================================
GLenum OpenGl_VertexBuffer::GetTarget() const
{
  return GL_ARRAY_BUFFER;
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::Create (const Handle(OpenGl_Context)& theGlCtx)
{
  if (myBufferId == NO_BUFFER)
  {
    theGlCtx->core15->glGenBuffers (1, &myBufferId);
  }
  return myBufferId != NO_BUFFER;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::Release (const OpenGl_Context* theGlCtx)
{
  if (myBufferId == NO_BUFFER)
  {
    return;
  }

  // application can not handle this case by exception - this is bug in code
  Standard_ASSERT_RETURN (theGlCtx != NULL,
    "OpenGl_VertexBuffer destroyed without GL context! Possible GPU memory leakage...",);

  theGlCtx->core15->glDeleteBuffers (1, &myBufferId);
  myBufferId = NO_BUFFER;
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::Bind (const Handle(OpenGl_Context)& theGlCtx) const
{
  theGlCtx->core15->glBindBuffer (GetTarget(), myBufferId);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::Unbind (const Handle(OpenGl_Context)& theGlCtx) const
{
  theGlCtx->core15->glBindBuffer (GetTarget(), NO_BUFFER);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                                const GLuint   theComponentsNb,
                                const GLsizei  theElemsNb,
                                const GLfloat* theData)
{
  if (!Create (theGlCtx))
  {
    return false;
  }

  Bind (theGlCtx);
  myDataType     = GL_FLOAT;
  myComponentsNb = theComponentsNb;
  myElemsNb      = theElemsNb;
  theGlCtx->core15->glBufferData (GetTarget(), GLsizeiptr(myElemsNb) * GLsizeiptr(myComponentsNb) * sizeof(GLfloat), theData, GL_STATIC_DRAW);
  bool isDone = (glGetError() == GL_NO_ERROR); // GL_OUT_OF_MEMORY
  Unbind (theGlCtx);
  return isDone;
}

// =======================================================================
// function : SubData
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::SubData (const Handle(OpenGl_Context)& theGlCtx,
                                   const GLsizei  theElemFrom,
                                   const GLsizei  theElemsNb,
                                   const GLfloat* theData)
{
  if (!IsValid() || myDataType != GL_FLOAT ||
      theElemFrom < 0 || ((theElemFrom + theElemsNb) > myElemsNb))
  {
    return false;
  }

  Bind (theGlCtx);
  theGlCtx->core15->glBufferSubData (GetTarget(),
                                     GLintptr(theElemFrom)  * GLintptr(myComponentsNb)   * sizeof(GLfloat), // offset in bytes
                                     GLsizeiptr(theElemsNb) * GLsizeiptr(myComponentsNb) * sizeof(GLfloat), // size   in bytes
                                     theData);
  bool isDone = (glGetError() == GL_NO_ERROR); // some dummy error
  Unbind (theGlCtx);
  return isDone;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                                const GLuint  theComponentsNb,
                                const GLsizei theElemsNb,
                                const GLuint* theData)
{
  if (!Create (theGlCtx))
  {
    return false;
  }

  Bind (theGlCtx);
  myDataType     = GL_UNSIGNED_INT;  
  myComponentsNb = theComponentsNb;
  myElemsNb      = theElemsNb;
  theGlCtx->core15->glBufferData (GetTarget(), GLsizeiptr(myElemsNb) * GLsizeiptr(myComponentsNb) * sizeof(GLuint), theData, GL_STATIC_DRAW);
  bool isDone = (glGetError() == GL_NO_ERROR); // GL_OUT_OF_MEMORY
  Unbind (theGlCtx);
  return isDone;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                                const GLuint   theComponentsNb,
                                const GLsizei  theElemsNb,
                                const GLubyte* theData)
{
  if (!Create (theGlCtx))
  {
    return false;
  }

  Bind (theGlCtx);
  myDataType     = GL_UNSIGNED_BYTE;
  myComponentsNb = theComponentsNb;
  myElemsNb      = theElemsNb;
  theGlCtx->core15->glBufferData (GetTarget(), GLsizeiptr(myElemsNb) * GLsizeiptr(myComponentsNb) * sizeof(GLubyte), theData, GL_STATIC_DRAW);
  bool isDone = (glGetError() == GL_NO_ERROR); // GL_OUT_OF_MEMORY
  Unbind (theGlCtx);
  return isDone;
}

// =======================================================================
// function : BindVertexAttrib
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::BindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                            const GLuint                  theAttribLoc) const
{
  if (!IsValid() || theAttribLoc == GLuint (-1))
  {
    return;
  }
  Bind (theGlCtx);
  theGlCtx->core20->glEnableVertexAttribArray (theAttribLoc);
  theGlCtx->core20->glVertexAttribPointer (theAttribLoc, GLint (myComponentsNb), myDataType, GL_FALSE, 0, NULL);
}

// =======================================================================
// function : UnbindVertexAttrib
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::UnbindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                              const GLuint                  theAttribLoc) const
{
  if (!IsValid() || theAttribLoc == GLuint (-1))
  {
    return;
  }
  theGlCtx->core20->glDisableVertexAttribArray (theAttribLoc);
  Unbind (theGlCtx);
}

// =======================================================================
// function : BindFixed
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::BindFixed (const Handle(OpenGl_Context)& theGlCtx,
                                     const GLenum                  theMode) const
{
  if (!IsValid())
  {
    return;
  }

  Bind (theGlCtx);
  glEnableClientState (theMode);
  switch (theMode)
  {
    case GL_VERTEX_ARRAY:
    {
      glVertexPointer (static_cast<GLint> (myComponentsNb), myDataType, 0, NULL);
      break;
    }
    case GL_NORMAL_ARRAY:
    {
      glNormalPointer (myDataType, 0, NULL);
      break;
    }
    case GL_TEXTURE_COORD_ARRAY:
    {
      glTexCoordPointer (static_cast<GLint> (myComponentsNb), myDataType, 0, NULL);
      break;
    }
    case GL_COLOR_ARRAY:
    {
      glColorPointer (static_cast<GLint> (myComponentsNb), myDataType, 0, NULL);
      break;
    }
    default: break;
  }
}

// =======================================================================
// function : UnbindFixed
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::UnbindFixed (const Handle(OpenGl_Context)& theGlCtx,
                                       const GLenum                  theMode) const
{
  if (!IsValid())
  {
    return;
  }
  Unbind (theGlCtx);
  glDisableClientState (theMode);
}
