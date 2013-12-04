// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_PrimitiveArray_Header
#define OpenGl_PrimitiveArray_Header

#include <OpenGl_VertexBuffer.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfMarker.hxx>

#include <OpenGl_Element.hxx>

struct OPENGL_SURF_PROP;

class OpenGl_PrimitiveArray : public OpenGl_Element
{
public:
  // OpenGL does not provie a constant for "none" draw mode.
  // So we define our own one that does not conflict with GL constants
  // and untilizes common GL invalid value
  enum
  {
    DRAW_MODE_NONE = -1
  };

  //! Default constructor
  OpenGl_PrimitiveArray (CALL_DEF_PARRAY* thePArray);

  //! Render primitives to the window
  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;

  virtual void Release (const Handle(OpenGl_Context)&   theContext);

  CALL_DEF_PARRAY* PArray() const { return myPArray; }

private:

  Standard_Boolean toDrawVbo() const
  {
    return !myVbos[VBOVertices].IsNull();
  }

  //! VBO initialization procedures
  Standard_Boolean BuildVBO (const Handle(OpenGl_Workspace)& theWorkspace) const;
  void clearMemoryOwn() const;
  void clearMemoryGL (const Handle(OpenGl_Context)& theGlCtx) const;

  //! Main procedure to draw array
  void DrawArray (Tint theLightingModel,
                  const Aspect_InteriorStyle theInteriorStyle,
                  Tint theEdgeFlag,
                  const TEL_COLOUR* theInteriorColour,
                  const TEL_COLOUR* theLineColour,
                  const TEL_COLOUR* theEdgeColour,
                  const OPENGL_SURF_PROP* theFaceProp,
                  const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Auxiliary procedures
  void DrawEdges (const TEL_COLOUR*               theEdgeColour,
                  const Handle(OpenGl_Workspace)& theWorkspace) const;

  void DrawMarkers (const Handle(OpenGl_Workspace)& theWorkspace) const;

protected:

  //! Destructor
  virtual ~OpenGl_PrimitiveArray();

protected:

  typedef enum
  {
    VBOEdges,
    VBOVertices,
    VBOVcolours,
    VBOVnormals,
    VBOVtexels,
    VBOMaxType
  } VBODataType;

  mutable CALL_DEF_PARRAY*            myPArray;
  mutable Handle(OpenGl_VertexBuffer) myVbos[VBOMaxType];
  GLint                               myDrawMode;
  mutable Standard_Boolean            myIsVboInit;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_PrimitiveArray_Header
