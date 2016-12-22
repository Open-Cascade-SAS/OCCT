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

#include <OpenGl_VertexBuffer.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_VertexBuffer,OpenGl_Resource)

// =======================================================================
// function : OpenGl_VertexBuffer
// purpose  :
// =======================================================================
OpenGl_VertexBuffer::OpenGl_VertexBuffer()
: OpenGl_Resource(),
  myOffset (NULL),
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
    theGlCtx->core15fwd->glGenBuffers (1, &myBufferId);
  }
  return myBufferId != NO_BUFFER;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::Release (OpenGl_Context* theGlCtx)
{
  if (myBufferId == NO_BUFFER)
  {
    return;
  }

  // application can not handle this case by exception - this is bug in code
  Standard_ASSERT_RETURN (theGlCtx != NULL,
    "OpenGl_VertexBuffer destroyed without GL context! Possible GPU memory leakage...",);

  if (theGlCtx->IsValid())
  {
    theGlCtx->core15fwd->glDeleteBuffers (1, &myBufferId);
  }
  myOffset   = NULL;
  myBufferId = NO_BUFFER;
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::Bind (const Handle(OpenGl_Context)& theGlCtx) const
{
  theGlCtx->core15fwd->glBindBuffer (GetTarget(), myBufferId);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::Unbind (const Handle(OpenGl_Context)& theGlCtx) const
{
  theGlCtx->core15fwd->glBindBuffer (GetTarget(), NO_BUFFER);
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::init (const Handle(OpenGl_Context)& theGlCtx,
                                const GLuint   theComponentsNb,
                                const GLsizei  theElemsNb,
                                const void*    theData,
                                const GLenum   theDataType,
                                const GLsizei  theStride)
{
  if (!Create (theGlCtx))
  {
    return false;
  }

  Bind (theGlCtx);
  myDataType     = theDataType;
  myComponentsNb = theComponentsNb;
  myElemsNb      = theElemsNb;
  theGlCtx->core15fwd->glBufferData (GetTarget(), GLsizeiptr(myElemsNb) * theStride, theData, GL_STATIC_DRAW);
  bool isDone = (glGetError() == GL_NO_ERROR); // GL_OUT_OF_MEMORY
  Unbind (theGlCtx);
  return isDone;
}

// =======================================================================
// function : subData
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::subData (const Handle(OpenGl_Context)& theGlCtx,
                                   const GLsizei  theElemFrom,
                                   const GLsizei  theElemsNb,
                                   const void*    theData,
                                   const GLenum   theDataType)
{
  if (!IsValid() || myDataType != theDataType ||
      theElemFrom < 0 || ((theElemFrom + theElemsNb) > myElemsNb))
  {
    return false;
  }

  Bind (theGlCtx);
  const size_t aDataSize = sizeOfGlType (theDataType);
  theGlCtx->core15fwd->glBufferSubData (GetTarget(),
                                        GLintptr(theElemFrom)  * GLintptr  (myComponentsNb) * aDataSize, // offset in bytes
                                        GLsizeiptr(theElemsNb) * GLsizeiptr(myComponentsNb) * aDataSize, // size   in bytes
                                        theData);
  bool isDone = (glGetError() == GL_NO_ERROR); // some dummy error
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
  theGlCtx->core20fwd->glEnableVertexAttribArray (theAttribLoc);
  theGlCtx->core20fwd->glVertexAttribPointer (theAttribLoc, GLint (myComponentsNb), myDataType, GL_FALSE, 0, myOffset);
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
  theGlCtx->core20fwd->glDisableVertexAttribArray (theAttribLoc);
  Unbind (theGlCtx);
}

// =======================================================================
// function : BindAllAttributes
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::BindAllAttributes (const Handle(OpenGl_Context)& ) const
{
  //
}

// =======================================================================
// function : BindPositionAttribute
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::BindPositionAttribute (const Handle(OpenGl_Context)& ) const
{
  //
}

// =======================================================================
// function : UnbindAllAttributes
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::UnbindAllAttributes (const Handle(OpenGl_Context)& ) const
{
  //
}

// =======================================================================
// function : HasColorAttribute
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::HasColorAttribute() const
{
  return false;
}

// =======================================================================
// function : HasNormalAttribute
// purpose  :
// =======================================================================
bool OpenGl_VertexBuffer::HasNormalAttribute() const
{
  return false;
}

#if !defined(GL_ES_VERSION_2_0)
// =======================================================================
// function : unbindFixedColor
// purpose  :
// =======================================================================
void OpenGl_VertexBuffer::unbindFixedColor (const Handle(OpenGl_Context)& theCtx)
{
  theCtx->core11->glDisableClientState (GL_COLOR_ARRAY);
  theCtx->core11fwd->glDisable (GL_COLOR_MATERIAL);

  // invalidate FFP material state after GL_COLOR_MATERIAL has modified it (took values from the vertex color)
  theCtx->ShaderManager()->UpdateMaterialState();
}
#endif
