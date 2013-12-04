// Created by: Kirill GAVRILOV
// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_VertexBuffer_H__
#define _OpenGl_VertexBuffer_H__

#include <OpenGl_GlCore20.hxx>
#include <OpenGl_Resource.hxx>

class Handle(OpenGl_Context);
class OpenGl_Context;

//! Vertex Buffer Object - is a general storage object for vertex attributes (position, normal, color).
//! Notice that you should use OpenGl_IndexBuffer specialization for array of indices.
class OpenGl_VertexBuffer : public OpenGl_Resource
{

public:

  //! Helpful constants
  static const GLuint NO_BUFFER = 0;

public:

  //! Create uninitialized VBO.
  Standard_EXPORT OpenGl_VertexBuffer();

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_VertexBuffer();

  Standard_EXPORT virtual GLenum GetTarget() const;

  //! @return true if current object was initialized
  inline bool IsValid() const
  {
    return myBufferId != NO_BUFFER;
  }

  //! @return the number of components per generic vertex attribute.
  inline GLuint GetComponentsNb() const
  {
    return myComponentsNb;
  }

  //! @return number of vertex attributes / number of vertices.
  inline GLsizei GetElemsNb() const
  {
    return myElemsNb;
  }

  //! @return data type of each component in the array.
  inline GLenum GetDataType() const
  {
    return myDataType;
  }

  //! Creates VBO name (id) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create (const Handle(OpenGl_Context)& theGlCtx);

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theGlCtx);

  //! Bind this VBO.
  Standard_EXPORT void Bind (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Unbind this VBO.
  Standard_EXPORT void Unbind (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLfloat data (vertices/normals etc.).
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theGlCtx,
                             const GLuint   theComponentsNb,
                             const GLsizei  theElemsNb,
                             const GLfloat* theData);

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLuint data (indices etc.).
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theGlCtx,
                             const GLuint  theComponentsNb,
                             const GLsizei theElemsNb,
                             const GLuint* theData);

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLubyte data (indices/colors etc.).
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theGlCtx,
                             const GLuint   theComponentsNb,
                             const GLsizei  theElemsNb,
                             const GLubyte* theData);

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom - element id from which replace buffer data (>=0);
  //! @param theElemsNb  - elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     - pointer to GLfloat data.
  Standard_EXPORT bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                                const GLsizei  theElemFrom,
                                const GLsizei  theElemsNb,
                                const GLfloat* theData);

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom element id from which replace buffer data (>=0);
  //! @param theElemsNb  elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     pointer to GLuint data.
  Standard_EXPORT bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                                const GLsizei theElemFrom,
                                const GLsizei theElemsNb,
                                const GLuint* theData);

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom element id from which replace buffer data (>=0);
  //! @param theElemsNb  elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     pointer to GLubyte data.
  Standard_EXPORT bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                                const GLsizei  theElemFrom,
                                const GLsizei  theElemsNb,
                                const GLubyte* theData);

  //! Bind this VBO to active GLSL program.
  Standard_EXPORT void BindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                         const GLuint                  theAttribLoc) const;

  //! Unbind any VBO from active GLSL program.
  Standard_EXPORT void UnbindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                           const GLuint                  theAttribLoc) const;

  //! Bind this VBO as fixed pipeline attribute.
  //! @param theGlCtx - handle to bound GL context;
  //! @param theMode  - array mode (GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_COLOR_ARRAY, GL_INDEX_ARRAY, GL_TEXTURE_COORD_ARRAY).
  Standard_EXPORT void BindFixed (const Handle(OpenGl_Context)& theGlCtx,
                                  const GLenum                  theMode) const;

  //! Unbind this VBO as fixed pipeline attribute.
  //! @param theGlCtx - handle to bound GL context;
  //! @param theMode  - array mode.
  Standard_EXPORT void UnbindFixed (const Handle(OpenGl_Context)& theGlCtx,
                                    const GLenum                  theMode) const;

protected:

  GLuint  myBufferId;     //!< VBO name (index)
  GLuint  myComponentsNb; //!< Number of components per generic vertex attribute, must be 1, 2, 3, or 4
  GLsizei myElemsNb;      //!< Number of vertex attributes / number of vertices
  GLenum  myDataType;     //!< Data type (GL_FLOAT, GL_UNSIGNED_INT, GL_UNSIGNED_BYTE etc.)

public:

  DEFINE_STANDARD_RTTI(OpenGl_VertexBuffer) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_VertexBuffer, OpenGl_Resource)

#endif // _OpenGl_VertexBuffer_H__
