// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_PrimitiveArray_Header
#define OpenGl_PrimitiveArray_Header

#include <OpenGl_IndexBuffer.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_BoundBuffer.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_GraphicDriver;
class Handle(OpenGl_ShaderProgram);

//! Class for rendering of arbitrary primitive array.
class OpenGl_PrimitiveArray : public OpenGl_Element
{
public:
  // OpenGL does not provide a constant for "none" draw mode.
  // So we define our own one that does not conflict with GL constants
  // and utilizes common GL invalid value
  enum
  {
    DRAW_MODE_NONE = -1
  };

  //! Default constructor
  OpenGl_PrimitiveArray (const OpenGl_GraphicDriver*          theDriver,
                         const Graphic3d_TypeOfPrimitiveArray theType,
                         const Handle(Graphic3d_IndexBuffer)& theIndices,
                         const Handle(Graphic3d_Buffer)&      theAttribs,
                         const Handle(Graphic3d_BoundBuffer)& theBounds);

  //! Render primitives to the window
  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;

  virtual void Release (OpenGl_Context* theContext);

  //! @return primitive type (GL_LINES, GL_TRIANGLES and others)
  GLint DrawMode() const { return myDrawMode; }

  //! @return indices array
  const Handle(Graphic3d_IndexBuffer)& Indices() const { return myIndices; }

  //! @return attributes array
  const Handle(Graphic3d_Buffer)& Attributes() const { return myAttribs; }

  //! @return bounds array
  const Handle(Graphic3d_BoundBuffer)& Bounds() const { return myBounds; }

  //! Returns unique ID of primitive array. 
  const Standard_Size GetUID() const { return myUID; }

private:

  //! Initialize normal (OpenGL-provided) VBO
  Standard_Boolean initNormalVbo (const Handle(OpenGl_Context)& theCtx) const;

  //! VBO initialization procedures
  //! @param theCtx        bound GL context
  //! @param theToKeepData when true, myAttribs will not be nullified after VBO creation
  Standard_Boolean buildVBO (const Handle(OpenGl_Context)& theCtx,
                             const Standard_Boolean        theToKeepData) const;
  void clearMemoryGL (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Main procedure to draw array
  void drawArray (const Handle(OpenGl_Workspace)& theWorkspace,
                  const Graphic3d_Vec4*           theFaceColors,
                  const Standard_Boolean          theHasVertColor) const;

  //! Auxiliary procedures
  void drawEdges (const TEL_COLOUR*               theEdgeColour,
                  const Handle(OpenGl_Workspace)& theWorkspace) const;

  void drawMarkers (const Handle(OpenGl_Workspace)& theWorkspace) const;

protected:

  //! Destructor
  virtual ~OpenGl_PrimitiveArray();

protected:

  mutable Handle(OpenGl_VertexBuffer)   myVboIndices;
  mutable Handle(OpenGl_VertexBuffer)   myVboAttribs;

  mutable Handle(Graphic3d_IndexBuffer) myIndices;
  mutable Handle(Graphic3d_Buffer)      myAttribs;
  mutable Handle(Graphic3d_BoundBuffer) myBounds;
  GLint                                 myDrawMode;
  mutable Standard_Boolean              myIsVboInit;

  Standard_Size                         myUID; //!< Unique ID of primitive array. 

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_PrimitiveArray_Header
