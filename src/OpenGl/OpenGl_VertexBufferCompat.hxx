// Created by: Kirill GAVRILOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_VertexBufferCompat_HeaderFile
#define _OpenGl_VertexBufferCompat_HeaderFile

#include <OpenGl_VertexBuffer.hxx>

//! Compatibility layer for old OpenGL without VBO.
//! Make sure to pass pointer from GetDataOffset() instead of NULL.
//! Method GetDataOffset() returns pointer to real data in this class
//! (while base class OpenGl_VertexBuffer always return NULL).
//!
//! Methods Bind()/Unbind() do nothing (do not affect OpenGL state)
//! and ::GetTarget() is never used.
//! For this reason there is no analog for OpenGl_IndexBuffer.
//! Just pass GetDataOffset() to glDrawElements() directly as last argument.
//!
//! Class overrides methods init() and subData() to copy data into own memory buffer.
//! Extra method initLink() might be used to pass existing buffer through handle without copying the data.
//!
//! Method Create() creates dummy identifier for this object which should NOT be passed to OpenGL functions.
class OpenGl_VertexBufferCompat : public OpenGl_VertexBuffer
{

public:

  //! Create uninitialized VBO.
  Standard_EXPORT OpenGl_VertexBufferCompat();

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_VertexBufferCompat();

  //! Creates VBO name (id) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create (const Handle(OpenGl_Context)& theGlCtx) Standard_OVERRIDE;

  //! Destroy object - will release memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Bind this VBO.
  Standard_EXPORT virtual void Bind (const Handle(OpenGl_Context)& theGlCtx) const Standard_OVERRIDE;

  //! Unbind this VBO.
  Standard_EXPORT virtual void Unbind (const Handle(OpenGl_Context)& theGlCtx) const Standard_OVERRIDE;

public: //! @name advanced methods

  //! Initialize buffer with existing data.
  //! Data will NOT be copied by this method!
  Standard_EXPORT bool initLink (const Handle(NCollection_Buffer)& theData,
                                 const GLuint   theComponentsNb,
                                 const GLsizei  theElemsNb,
                                 const GLenum   theDataType);

  //! Initialize buffer with new data (data will be copied).
  Standard_EXPORT virtual bool init (const Handle(OpenGl_Context)& theGlCtx,
                                     const GLuint   theComponentsNb,
                                     const GLsizei  theElemsNb,
                                     const void*    theData,
                                     const GLenum   theDataType,
                                     const GLsizei  theStride) Standard_OVERRIDE;

  //! Update part of the buffer with new data.
  Standard_EXPORT virtual bool subData (const Handle(OpenGl_Context)& theGlCtx,
                                        const GLsizei  theElemFrom,
                                        const GLsizei  theElemsNb,
                                        const void*    theData,
                                        const GLenum   theDataType) Standard_OVERRIDE;

protected:

  Handle(NCollection_Buffer) myData; //!< buffer data

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_VertexBufferCompat,OpenGl_VertexBuffer) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_VertexBufferCompat, OpenGl_VertexBuffer)

#endif // _OpenGl_VertexBufferCompat_HeaderFile
