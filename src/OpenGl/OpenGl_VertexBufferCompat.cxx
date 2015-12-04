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

#include <OpenGl_VertexBufferCompat.hxx>

#include <NCollection_AlignedAllocator.hxx>


IMPLEMENT_STANDARD_RTTIEXT(OpenGl_VertexBufferCompat,OpenGl_VertexBuffer)

// =======================================================================
// function : OpenGl_VertexBufferCompat
// purpose  :
// =======================================================================
OpenGl_VertexBufferCompat::OpenGl_VertexBufferCompat()
{
  //
}

// =======================================================================
// function : ~OpenGl_VertexBufferCompat
// purpose  :
// =======================================================================
OpenGl_VertexBufferCompat::~OpenGl_VertexBufferCompat()
{
  Release (NULL);
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool OpenGl_VertexBufferCompat::Create (const Handle(OpenGl_Context)& )
{
  if (myBufferId == NO_BUFFER)
  {
    myBufferId = (GLuint )-1; // dummy identifier...
    Handle(NCollection_AlignedAllocator) anAlloc = new NCollection_AlignedAllocator (16);
    myData = new NCollection_Buffer (anAlloc);
  }
  return myBufferId != NO_BUFFER;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_VertexBufferCompat::Release (OpenGl_Context* )
{
  if (myBufferId == NO_BUFFER)
  {
    return;
  }

  myOffset   = NULL;
  myBufferId = NO_BUFFER;
  myData.Nullify();
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void OpenGl_VertexBufferCompat::Bind (const Handle(OpenGl_Context)& ) const
{
  //
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_VertexBufferCompat::Unbind (const Handle(OpenGl_Context)& ) const
{
  //
}

// =======================================================================
// function : initLink
// purpose  :
// =======================================================================
bool OpenGl_VertexBufferCompat::initLink (const Handle(NCollection_Buffer)& theData,
                                          const GLuint   theComponentsNb,
                                          const GLsizei  theElemsNb,
                                          const GLenum   theDataType)
{
  if (theData.IsNull())
  {
    myOffset = NULL;
    return false;
  }

  if (myBufferId == NO_BUFFER)
  {
    myBufferId = (GLuint )-1; // dummy identifier...
  }
  myData         = theData;
  myDataType     = theDataType;
  myComponentsNb = theComponentsNb;
  myElemsNb      = theElemsNb;
  myOffset       = myData->ChangeData();
  return true;
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
bool OpenGl_VertexBufferCompat::init (const Handle(OpenGl_Context)& theCtx,
                                      const GLuint   theComponentsNb,
                                      const GLsizei  theElemsNb,
                                      const void*    theData,
                                      const GLenum   theDataType,
                                      const GLsizei  theStride)
{
  if (!Create (theCtx))
  {
    myOffset = NULL;
    return false;
  }

  myDataType     = theDataType;
  myComponentsNb = theComponentsNb;
  myElemsNb      = theElemsNb;

  const size_t aNbBytes = size_t(myElemsNb) * theStride;
  if (!myData->Allocate (aNbBytes))
  {
    myOffset = NULL;
    return false;
  }

  myOffset = myData->ChangeData();
  if (theData != NULL)
  {
    memcpy (myData->ChangeData(), theData, aNbBytes);
  }
  return true;
}

// =======================================================================
// function : subData
// purpose  :
// =======================================================================
bool OpenGl_VertexBufferCompat::subData (const Handle(OpenGl_Context)& ,
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
  else if (theData == NULL)
  {
    return true;
  }

  const size_t aDataSize = sizeOfGlType (theDataType);
  const size_t anOffset  = size_t(theElemFrom) * size_t(myComponentsNb) * aDataSize;
  const size_t aNbBytes  = size_t(theElemsNb)  * size_t(myComponentsNb) * aDataSize;
  memcpy (myData->ChangeData() + anOffset, theData, aNbBytes);
  return true;
}
