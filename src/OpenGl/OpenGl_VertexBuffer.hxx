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

#ifndef OpenGl_VertexBuffer_HeaderFile
#define OpenGl_VertexBuffer_HeaderFile

#include <OpenGl_Buffer.hxx>
#include <Graphic3d_Buffer.hxx>

//! Vertex Buffer Object - is a general storage object for vertex attributes (position, normal, color).
//! Notice that you should use OpenGl_IndexBuffer specialization for array of indices.
class OpenGl_VertexBuffer : public OpenGl_Buffer
{
public:

  //! Create uninitialized VBO.
  Standard_EXPORT OpenGl_VertexBuffer();

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_VertexBuffer();

  //! Return buffer target GL_ARRAY_BUFFER.
  Standard_EXPORT virtual unsigned int GetTarget() const Standard_OVERRIDE;

  //! Bind this VBO to active GLSL program.
  Standard_EXPORT void BindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                         const unsigned int            theAttribLoc) const;

  //! Unbind any VBO from active GLSL program.
  Standard_EXPORT void UnbindVertexAttrib (const Handle(OpenGl_Context)& theGlCtx,
                                           const unsigned int            theAttribLoc) const;

  //! Bind this VBO and enable specified attribute in OpenGl_Context::ActiveProgram() or FFP.
  //! @param theGlCtx - handle to bound GL context;
  //! @param theMode  - array mode (GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_COLOR_ARRAY, GL_INDEX_ARRAY, GL_TEXTURE_COORD_ARRAY).
  void BindAttribute (const Handle(OpenGl_Context)&   theCtx,
                      const Graphic3d_TypeOfAttribute theMode) const
  {
    if (IsValid())
    {
      Bind (theCtx);
      bindAttribute (theCtx, theMode, static_cast<int> (myComponentsNb), myDataType, 0, myOffset);
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

  //! Setup array pointer - either for active GLSL program OpenGl_Context::ActiveProgram()
  //! or for FFP using bindFixed() when no program bound.
  Standard_EXPORT static void bindAttribute (const Handle(OpenGl_Context)&   theGlCtx,
                                             const Graphic3d_TypeOfAttribute theMode,
                                             const Standard_Integer          theNbComp,
                                             const unsigned int              theDataType,
                                             const Standard_Integer          theStride,
                                             const void*                     theOffset);

  //! Disable GLSL array pointer - either for active GLSL program OpenGl_Context::ActiveProgram()
  //! or for FFP using unbindFixed() when no program bound.
  Standard_EXPORT static void unbindAttribute (const Handle(OpenGl_Context)&   theGlCtx,
                                               const Graphic3d_TypeOfAttribute theMode);

private:

  //! Setup FFP array pointer.
  Standard_EXPORT static void bindFixed (const Handle(OpenGl_Context)&   theGlCtx,
                                         const Graphic3d_TypeOfAttribute theMode,
                                         const Standard_Integer          theNbComp,
                                         const unsigned int              theDataType,
                                         const Standard_Integer          theStride,
                                         const void*                     theOffset);

  //! Disable FFP array pointer.
  Standard_EXPORT static void unbindFixed (const Handle(OpenGl_Context)&   theGlCtx,
                                           const Graphic3d_TypeOfAttribute theMode);

  //! Disable FFP color array pointer.
  Standard_EXPORT static void unbindFixedColor (const Handle(OpenGl_Context)& theCtx);

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

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_VertexBuffer, OpenGl_Buffer)

};

DEFINE_STANDARD_HANDLE(OpenGl_VertexBuffer, OpenGl_Buffer)

#endif // _OpenGl_VertexBuffer_H__
