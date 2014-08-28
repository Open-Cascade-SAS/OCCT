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

  //! Creates VBO name (id) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create (const Handle(OpenGl_Context)& theGlCtx);

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx);

  //! Bind this VBO.
  Standard_EXPORT void Bind (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Unbind this VBO.
  Standard_EXPORT void Unbind (const Handle(OpenGl_Context)& theGlCtx) const;

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
      case GL_INT:
      case GL_UNSIGNED_INT:   return sizeof(GLuint);
      case GL_FLOAT:          return sizeof(GLfloat);
      case GL_DOUBLE:         return sizeof(GLdouble);
      default:                return 0;
    }
  }

  //! Initialize buffer with new data.
  Standard_EXPORT bool init (const Handle(OpenGl_Context)& theGlCtx,
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
  Standard_EXPORT bool subData (const Handle(OpenGl_Context)& theGlCtx,
                                const GLsizei  theElemFrom,
                                const GLsizei  theElemsNb,
                                const void*    theData,
                                const GLenum   theDataType);

  //! Setup FFP array pointer.
  static void bindFixed (const Handle(OpenGl_Context)&   theGlCtx,
                         const Graphic3d_TypeOfAttribute theMode,
                         const GLint                     theNbComp,
                         const GLenum                    theDataType,
                         const GLsizei                   theStride,
                         const GLvoid*                   theOffset)
  {
    switch (theMode)
    {
      case Graphic3d_TOA_POS:
      {
        theGlCtx->core11->glEnableClientState (GL_VERTEX_ARRAY);
        theGlCtx->core11->glVertexPointer (theNbComp, theDataType, theStride, theOffset);
        break;
      }
      case Graphic3d_TOA_NORM:
      {
        theGlCtx->core11->glEnableClientState (GL_NORMAL_ARRAY);
        theGlCtx->core11->glNormalPointer (theDataType, theStride, theOffset);
        break;
      }
      case Graphic3d_TOA_UV:
      {
        theGlCtx->core11->glEnableClientState (GL_TEXTURE_COORD_ARRAY);
        theGlCtx->core11->glTexCoordPointer (theNbComp, theDataType, theStride, theOffset);
        break;
      }
      case Graphic3d_TOA_COLOR:
      {
        theGlCtx->core11->glEnableClientState (GL_COLOR_ARRAY);
        theGlCtx->core11->glColorPointer (theNbComp, theDataType, theStride, theOffset);
        theGlCtx->core11->glColorMaterial (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        theGlCtx->core11fwd->glEnable (GL_COLOR_MATERIAL);
        break;
      }
      case Graphic3d_TOA_CUSTOM:
      {
        break;
      }
    }
  }

  //! Disable FFP array pointer.
  static void unbindFixed (const Handle(OpenGl_Context)&   theGlCtx,
                           const Graphic3d_TypeOfAttribute theMode)
  {
    switch (theMode)
    {
      case Graphic3d_TOA_POS:    theGlCtx->core11->glDisableClientState (GL_VERTEX_ARRAY);        break;
      case Graphic3d_TOA_NORM:   theGlCtx->core11->glDisableClientState (GL_NORMAL_ARRAY);        break;
      case Graphic3d_TOA_UV:     theGlCtx->core11->glDisableClientState (GL_TEXTURE_COORD_ARRAY); break;
      case Graphic3d_TOA_COLOR:
      {
        theGlCtx->core11->glDisableClientState (GL_COLOR_ARRAY);
        theGlCtx->core11fwd->glDisable (GL_COLOR_MATERIAL);
        break;
      }
      case Graphic3d_TOA_CUSTOM:
      {
        break;
      }
    }
  }

public: //! @name methods for interleaved attributes array

  //! Bind all vertex attributes. Default implementation does nothing.
  Standard_EXPORT virtual void BindFixed   (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Bind all vertex position attribute only. Default implementation does nothing.
  Standard_EXPORT virtual void BindFixedPosition (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Unbind all vertex attributes. Default implementation does nothing.
  Standard_EXPORT virtual void UnbindFixed (const Handle(OpenGl_Context)& theGlCtx) const;

  //! @return true if buffer contains per-vertex color attribute
  Standard_EXPORT virtual bool HasColorAttribute() const;

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
