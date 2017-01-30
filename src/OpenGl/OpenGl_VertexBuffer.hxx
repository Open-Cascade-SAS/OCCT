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

#ifndef _OpenGl_VertexBuffer_H__
#define _OpenGl_VertexBuffer_H__

#include <OpenGl_GlCore20.hxx>
#include <OpenGl_Resource.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderProgram.hxx>

#include <Graphic3d_IndexBuffer.hxx>

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

  //! @return offset to data, NULL by default
  inline GLubyte* GetDataOffset() const
  {
    return myOffset;
  }

  //! Creates VBO name (id) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT virtual bool Create (const Handle(OpenGl_Context)& theGlCtx);

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Bind this VBO.
  Standard_EXPORT virtual void Bind (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Unbind this VBO.
  Standard_EXPORT virtual void Unbind (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLfloat data (vertices/normals etc.).
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const GLuint   theComponentsNb,
             const GLsizei  theElemsNb,
             const GLfloat* theData)
  {
    return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_FLOAT);
  }

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLuint data (indices etc.).
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const GLuint  theComponentsNb,
             const GLsizei theElemsNb,
             const GLuint* theData)
  {
    return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_UNSIGNED_INT);
  }

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLushort data (indices etc.).
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const GLuint    theComponentsNb,
             const GLsizei   theElemsNb,
             const GLushort* theData)
  {
    return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_UNSIGNED_SHORT);
  }

  //! Notice that VBO will be unbound after this call.
  //! @param theComponentsNb - specifies the number of components per generic vertex attribute; must be 1, 2, 3, or 4;
  //! @param theElemsNb      - elements count;
  //! @param theData         - pointer to GLubyte data (indices/colors etc.).
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const GLuint   theComponentsNb,
             const GLsizei  theElemsNb,
             const GLubyte* theData)
  {
    return init (theGlCtx, theComponentsNb, theElemsNb, theData, GL_UNSIGNED_BYTE);
  }

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom - element id from which replace buffer data (>=0);
  //! @param theElemsNb  - elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     - pointer to GLfloat data.
  bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                const GLsizei  theElemFrom,
                const GLsizei  theElemsNb,
                const GLfloat* theData)
  {
    return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_FLOAT);
  }

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom element id from which replace buffer data (>=0);
  //! @param theElemsNb  elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     pointer to GLuint data.
  bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                const GLsizei theElemFrom,
                const GLsizei theElemsNb,
                const GLuint* theData)
  {
    return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_INT);
  }

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom element id from which replace buffer data (>=0);
  //! @param theElemsNb  elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     pointer to GLushort data.
  bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                const GLsizei   theElemFrom,
                const GLsizei   theElemsNb,
                const GLushort* theData)
  {
    return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_SHORT);
  }

  //! Notice that VBO will be unbound after this call.
  //! Function replaces portion of data within this VBO using glBufferSubData().
  //! The VBO should be initialized before call.
  //! @param theElemFrom element id from which replace buffer data (>=0);
  //! @param theElemsNb  elements count (theElemFrom + theElemsNb <= GetElemsNb());
  //! @param theData     pointer to GLubyte data.
  bool SubData (const Handle(OpenGl_Context)& theGlCtx,
                const GLsizei  theElemFrom,
                const GLsizei  theElemsNb,
                const GLubyte* theData)
  {
    return subData (theGlCtx, theElemFrom, theElemsNb, theData, GL_UNSIGNED_BYTE);
  }

  //! Bind this VBO to active GLSL program.
  Standard_EXPORT void BindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                         const GLuint                  theAttribLoc) const;

  //! Unbind any VBO from active GLSL program.
  Standard_EXPORT void UnbindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                           const GLuint                  theAttribLoc) const;

  //! Bind this VBO and enable specified attribute in OpenGl_Context::ActiveProgram() or FFP.
  //! @param theGlCtx - handle to bound GL context;
  //! @param theMode  - array mode (GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_COLOR_ARRAY, GL_INDEX_ARRAY, GL_TEXTURE_COORD_ARRAY).
  void BindAttribute (const Handle(OpenGl_Context)&   theCtx,
                      const Graphic3d_TypeOfAttribute theMode) const
  {
    if (IsValid())
    {
      Bind (theCtx);
      bindAttribute (theCtx, theMode, static_cast<GLint> (myComponentsNb), myDataType, 0, myOffset);
    }
  }

  //! Unbind this VBO and disable specified attribute in OpenGl_Context::ActiveProgram() or FFP.
  //! @param theCtx handle to bound GL context
  //! @param theMode  array mode
  void UnbindAttribute (const Handle(OpenGl_Context)&   theCtx,
                        const Graphic3d_TypeOfAttribute theMode) const
  {
    if (IsValid())
    {
      Unbind (theCtx);
      unbindAttribute (theCtx, theMode);
    }
  }

public: //! @name advanced methods

  //! @return size of specified GL type
  static size_t sizeOfGlType (const GLenum theType)
  {
    switch (theType)
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
      case GL_SHORT:
      case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    #ifdef GL_INT
      case GL_INT:
    #endif
      case GL_UNSIGNED_INT:   return sizeof(GLuint);
      case GL_FLOAT:          return sizeof(GLfloat);
    #ifdef GL_DOUBLE
      case GL_DOUBLE:         return sizeof(GLdouble);
    #endif
      default:                return 0;
    }
  }

  //! Initialize buffer with new data.
  Standard_EXPORT virtual bool init (const Handle(OpenGl_Context)& theGlCtx,
                                     const GLuint   theComponentsNb,
                                     const GLsizei  theElemsNb,
                                     const void*    theData,
                                     const GLenum   theDataType,
                                     const GLsizei  theStride);

  //! Initialize buffer with new data.
  bool init (const Handle(OpenGl_Context)& theGlCtx,
             const GLuint   theComponentsNb,
             const GLsizei  theElemsNb,
             const void*    theData,
             const GLenum   theDataType)
  {
    return init (theGlCtx, theComponentsNb, theElemsNb, theData, theDataType, GLsizei(theComponentsNb) * GLsizei(sizeOfGlType (theDataType)));
  }

  //! Update part of the buffer with new data.
  Standard_EXPORT virtual bool subData (const Handle(OpenGl_Context)& theGlCtx,
                                        const GLsizei  theElemFrom,
                                        const GLsizei  theElemsNb,
                                        const void*    theData,
                                        const GLenum   theDataType);

  //! Setup array pointer - either for active GLSL program OpenGl_Context::ActiveProgram()
  //! or for FFP using bindFixed() when no program bound.
  static void bindAttribute (const Handle(OpenGl_Context)&   theGlCtx,
                             const Graphic3d_TypeOfAttribute theMode,
                             const GLint                     theNbComp,
                             const GLenum                    theDataType,
                             const GLsizei                   theStride,
                             const GLvoid*                   theOffset);

  //! Disable GLSL array pointer - either for active GLSL program OpenGl_Context::ActiveProgram()
  //! or for FFP using unbindFixed() when no program bound.
  static void unbindAttribute (const Handle(OpenGl_Context)&   theGlCtx,
                               const Graphic3d_TypeOfAttribute theMode);

private:
#if !defined(GL_ES_VERSION_2_0)
  //! Setup FFP array pointer.
  static void bindFixed (const Handle(OpenGl_Context)&   theGlCtx,
                         const Graphic3d_TypeOfAttribute theMode,
                         const GLint                     theNbComp,
                         const GLenum                    theDataType,
                         const GLsizei                   theStride,
                         const GLvoid*                   theOffset);

  //! Disable FFP array pointer.
  static void unbindFixed (const Handle(OpenGl_Context)&   theGlCtx,
                           const Graphic3d_TypeOfAttribute theMode);

  //! Disable FFP color array pointer.
  Standard_EXPORT static void unbindFixedColor (const Handle(OpenGl_Context)& theCtx);

#endif
public: //! @name methods for interleaved attributes array

  //! @return true if buffer contains per-vertex color attribute
  Standard_EXPORT virtual bool HasColorAttribute() const;

  //! @return true if buffer contains per-vertex normal attribute
  Standard_EXPORT virtual bool HasNormalAttribute() const;

  //! Bind all vertex attributes to active program OpenGl_Context::ActiveProgram() or for FFP.
  //! Default implementation does nothing.
  Standard_EXPORT virtual void BindAllAttributes (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Bind vertex position attribute only. Default implementation does nothing.
  Standard_EXPORT virtual void BindPositionAttribute (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Unbind all vertex attributes. Default implementation does nothing.
  Standard_EXPORT virtual void UnbindAllAttributes (const Handle(OpenGl_Context)& theGlCtx) const;

protected:

  GLubyte* myOffset;       //!< offset to data
  GLuint   myBufferId;     //!< VBO name (index)
  GLuint   myComponentsNb; //!< Number of components per generic vertex attribute, must be 1, 2, 3, or 4
  GLsizei  myElemsNb;      //!< Number of vertex attributes / number of vertices
  GLenum   myDataType;     //!< Data type (GL_FLOAT, GL_UNSIGNED_INT, GL_UNSIGNED_BYTE etc.)

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_VertexBuffer,OpenGl_Resource) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_VertexBuffer, OpenGl_Resource)

#include <OpenGl_VertexBuffer.lxx>

#endif // _OpenGl_VertexBuffer_H__
