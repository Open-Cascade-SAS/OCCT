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

#include <OpenGl_Buffer.hxx>

#include <OpenGl_GlCore30.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Buffer, OpenGl_Resource)

// =======================================================================
// function : sizeOfGlType
// purpose  :
// =======================================================================
size_t OpenGl_Buffer::sizeOfGlType (unsigned int theType)
{
  switch (theType)
  {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:  return sizeof(Standard_Byte);
    case GL_SHORT:
    case GL_UNSIGNED_SHORT: return sizeof(unsigned short);
  #ifdef GL_INT
    case GL_INT:
  #endif
    case GL_UNSIGNED_INT:   return sizeof(unsigned int);
    case GL_FLOAT:          return sizeof(float);
  #ifdef GL_DOUBLE
    case GL_DOUBLE:         return sizeof(double);
  #endif
    default:                return 0;
  }
}

// =======================================================================
// function : FormatTarget
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Buffer::FormatTarget (unsigned int theTarget)
{
  switch (theTarget)
  {
    case GL_ARRAY_BUFFER:         return "GL_ARRAY_BUFFER";
    case GL_ELEMENT_ARRAY_BUFFER: return "GL_ELEMENT_ARRAY_BUFFER";
    case GL_PIXEL_UNPACK_BUFFER:  return "GL_PIXEL_UNPACK_BUFFER";
    case GL_PIXEL_PACK_BUFFER:    return "GL_PIXEL_PACK_BUFFER";
    case GL_UNIFORM_BUFFER:       return "GL_UNIFORM_BUFFER";
    case GL_TEXTURE_BUFFER:       return "GL_TEXTURE_BUFFER";
    case GL_COPY_READ_BUFFER:     return "GL_COPY_READ_BUFFER";
    case GL_COPY_WRITE_BUFFER:    return "GL_COPY_WRITE_BUFFER";
    case GL_TRANSFORM_FEEDBACK_BUFFER: return "GL_TRANSFORM_FEEDBACK_BUFFER";
  #ifdef GL_QUERY_BUFFER
    case GL_QUERY_BUFFER:         return "GL_QUERY_BUFFER";
    case GL_DRAW_INDIRECT_BUFFER: return "GL_DRAW_INDIRECT_BUFFER";
    case GL_ATOMIC_COUNTER_BUFFER: return "GL_ATOMIC_COUNTER_BUFFER";
    case GL_DISPATCH_INDIRECT_BUFFER: return "GL_DISPATCH_INDIRECT_BUFFER";
    case GL_SHADER_STORAGE_BUFFER: return "GL_SHADER_STORAGE_BUFFER";
  #endif
  }
  return OpenGl_Context::FormatGlEnumHex (theTarget);
}

// =======================================================================
// function : OpenGl_Buffer
// purpose  :
// =======================================================================
OpenGl_Buffer::OpenGl_Buffer()
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
// function : ~OpenGl_Buffer
// purpose  :
// =======================================================================
OpenGl_Buffer::~OpenGl_Buffer()
{
  Release (NULL);
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_Buffer::Create (const Handle(OpenGl_Context)& theGlCtx)
{
  if (myBufferId == NO_BUFFER && theGlCtx->core15fwd != NULL)
  {
    theGlCtx->core15fwd->glGenBuffers (1, &myBufferId);
  }
  return myBufferId != NO_BUFFER;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Buffer::Release (OpenGl_Context* theGlCtx)
{
  if (myBufferId == NO_BUFFER)
  {
    return;
  }

  // application can not handle this case by exception - this is bug in code
  Standard_ASSERT_RETURN (theGlCtx != NULL,
    "OpenGl_Buffer destroyed without GL context! Possible GPU memory leakage...",);

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
void OpenGl_Buffer::Bind (const Handle(OpenGl_Context)& theGlCtx) const
{
  theGlCtx->core15fwd->glBindBuffer (GetTarget(), myBufferId);
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_Buffer::Unbind (const Handle(OpenGl_Context)& theGlCtx) const
{
  theGlCtx->core15fwd->glBindBuffer (GetTarget(), NO_BUFFER);
}

// =======================================================================
// function : BindBufferBase
// purpose  :
// =======================================================================
void OpenGl_Buffer::BindBufferBase (const Handle(OpenGl_Context)& theGlCtx,
                                    unsigned int theIndex)
{
  theGlCtx->core30->glBindBufferBase (GetTarget(), theIndex, myBufferId);
}

// =======================================================================
// function : UnbindBufferBase
// purpose  :
// =======================================================================
void OpenGl_Buffer::UnbindBufferBase (const Handle(OpenGl_Context)& theGlCtx,
                                      unsigned int theIndex)
{
  theGlCtx->core30->glBindBufferBase (GetTarget(), theIndex, NO_BUFFER);
}

// =======================================================================
// function : BindBufferRange
// purpose  :
// =======================================================================
void OpenGl_Buffer::BindBufferRange (const Handle(OpenGl_Context)& theGlCtx,
                                     unsigned int   theIndex,
                                     const intptr_t theOffset,
                                     const size_t   theSize)
{
  theGlCtx->core30->glBindBufferRange (GetTarget(), theIndex, myBufferId, (GLintptr )theOffset, (GLsizeiptr )theSize);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Buffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                          const unsigned int     theComponentsNb,
                          const Standard_Integer theElemsNb,
                          const float*   theData)
{
  return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_FLOAT);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Buffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                          const unsigned int     theComponentsNb,
                          const Standard_Integer theElemsNb,
                          const unsigned int* theData)
{
  return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_UNSIGNED_INT);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Buffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                          const unsigned int     theComponentsNb,
                          const Standard_Integer theElemsNb,
                          const unsigned short*  theData)
{
  return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_UNSIGNED_SHORT);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool OpenGl_Buffer::Init (const Handle(OpenGl_Context)& theGlCtx,
                          const unsigned int     theComponentsNb,
                          const Standard_Integer theElemsNb,
                          const Standard_Byte*   theData)
{
  return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_UNSIGNED_BYTE);
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
bool OpenGl_Buffer::init (const Handle(OpenGl_Context)& theGlCtx,
                          const unsigned int     theComponentsNb,
                          const Standard_Integer theElemsNb,
                          const void*            theData,
                          const unsigned int     theDataType,
                          const Standard_Integer theStride)
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
  const int anErr = theGlCtx->core15fwd->glGetError();
  if (anErr != GL_NO_ERROR
   && anErr != GL_OUT_OF_MEMORY) // pass-through out-of-memory error, but log unexpected errors
  {
    theGlCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Error: glBufferData (")
                           + FormatTarget (GetTarget()) + ","
                           + OpenGl_Context::FormatSize (GLsizeiptr(myElemsNb) * theStride) + ","
                           + OpenGl_Context::FormatPointer (theData) + ") Id: " + (int )myBufferId
                           + " failed with " + OpenGl_Context::FormatGlError (anErr));
  }
  Unbind (theGlCtx);
  return anErr == GL_NO_ERROR;
}

// =======================================================================
// function : SubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::SubData (const Handle(OpenGl_Context)& theGlCtx,
                             const Standard_Integer theElemFrom,
                             const Standard_Integer theElemsNb,
                             const float* theData)
{
  return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_FLOAT);
}

// =======================================================================
// function : SubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::SubData (const Handle(OpenGl_Context)& theGlCtx,
                             const Standard_Integer theElemFrom,
                             const Standard_Integer theElemsNb,
                             const unsigned int* theData)
{
  return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_INT);
}

// =======================================================================
// function : SubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::SubData (const Handle(OpenGl_Context)& theGlCtx,
                             const Standard_Integer theElemFrom,
                             const Standard_Integer theElemsNb,
                             const unsigned short*  theData)
{
  return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_SHORT);
}

// =======================================================================
// function : SubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::SubData (const Handle(OpenGl_Context)& theGlCtx,
                             const Standard_Integer theElemFrom,
                             const Standard_Integer theElemsNb,
                             const Standard_Byte* theData)
{
  return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_BYTE);
}

// =======================================================================
// function : subData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::subData (const Handle(OpenGl_Context)& theGlCtx,
                             const Standard_Integer theElemFrom,
                             const Standard_Integer theElemsNb,
                             const void*            theData,
                             const unsigned int     theDataType)
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
  const int anErr = theGlCtx->core15fwd->glGetError();
  if (anErr != GL_NO_ERROR)
  {
    theGlCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           TCollection_AsciiString ("Error: glBufferSubData (")
                           + FormatTarget (GetTarget()) + ","
                           + OpenGl_Context::FormatSize (GLintptr(theElemFrom)  * GLintptr  (myComponentsNb) * aDataSize) + ","
                           + OpenGl_Context::FormatSize (GLsizeiptr(theElemsNb) * GLsizeiptr(myComponentsNb) * aDataSize) + ","
                           + OpenGl_Context::FormatPointer (theData) + ") Id: " + (int )myBufferId
                           + " failed with " + OpenGl_Context::FormatGlError (anErr));
  }
  Unbind (theGlCtx);
  return anErr == GL_NO_ERROR;
}

// =======================================================================
// function : subData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::GetSubData (const Handle(OpenGl_Context)& theGlCtx,
                                const Standard_Integer theElemFrom,
                                const Standard_Integer theElemsNb,
                                float* theData)
{
  return getSubData (theGlCtx, theElemFrom, theElemsNb, theData, GL_FLOAT);
}

// =======================================================================
// function : GetSubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::GetSubData (const Handle(OpenGl_Context)& theGlCtx,
                                const Standard_Integer theElemFrom,
                                const Standard_Integer theElemsNb,
                                unsigned short* theData)
{
  return getSubData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_SHORT);
}

// =======================================================================
// function : GetSubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::GetSubData (const Handle(OpenGl_Context)& theGlCtx,
                                const Standard_Integer theElemFrom,
                                const Standard_Integer theElemsNb,
                                unsigned int* theData)
{
  return getSubData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_INT);
}

// =======================================================================
// function : GetSubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::GetSubData (const Handle(OpenGl_Context)& theGlCtx,
                                const Standard_Integer theElemFrom,
                                const Standard_Integer theElemsNb,
                                Standard_Byte* theData)
{
  return getSubData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_BYTE);
}

// =======================================================================
// function : getSubData
// purpose  :
// =======================================================================
bool OpenGl_Buffer::getSubData (const Handle(OpenGl_Context)& theGlCtx,
                                const Standard_Integer theElemFrom,
                                const Standard_Integer theElemsNb,
                                void*                  theData,
                                const unsigned int     theDataType)
{
  if (!IsValid() || myDataType != theDataType
   || theElemFrom < 0 || ((theElemFrom + theElemsNb) > myElemsNb)
   || !theGlCtx->hasGetBufferData)
  {
    return false;
  }

  Bind (theGlCtx);
  const size_t  aDataSize = sizeOfGlType (theDataType);
  const GLintptr anOffset = GLintptr (theElemFrom) * GLintptr  (myComponentsNb) * aDataSize;
  const GLsizeiptr  aSize = GLsizeiptr(theElemsNb) * GLsizeiptr(myComponentsNb) * aDataSize;
  bool isDone = theGlCtx->GetBufferSubData (GetTarget(), anOffset, aSize, theData);
  isDone = isDone && (theGlCtx->core15fwd->glGetError() == GL_NO_ERROR);
  Unbind (theGlCtx);
  return isDone;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Buffer::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, OpenGl_Resource)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, GetTarget())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myOffset)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myBufferId)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myComponentsNb)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myElemsNb)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDataType)
}
