// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef OpenGl_PrimitiveArray_Header
#define OpenGl_PrimitiveArray_Header

#include <OpenGl_VertexBuffer.hxx>

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_InteriorStyle.hxx>

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

  void DrawDegeneratesAsPoints (const TEL_COLOUR* theEdgeColour,
                                const float       theSkipRatio) const;
  void DrawDegeneratesAsLines (const TEL_COLOUR*               theEdgeColour,
                               const Handle(OpenGl_Workspace)& theWorkspace) const;
  void DrawDegeneratesAsBBoxs (const TEL_COLOUR* theEdgeColour) const;

  void DrawDegeneratesPointsAsPoints() const;
  void DrawDegeneratesLinesAsPoints() const;
  void DrawDegeneratesLinesAsLines (const float theSkipRatio) const;
  void DrawDegeneratesTrianglesAsPoints() const;
  void DrawDegeneratesTrianglesAsLines (const float theSkipRatio) const;
  void DrawDegeneratesTrianglestripsAsPoints() const;
  void DrawDegeneratesTrianglestripsAsLines (const float theSkipRatio) const;
  void DrawDegeneratesQuadranglesAsPoints() const;
  void DrawDegeneratesQuadranglesAsLines (const float theSkipRatio) const;
  void DrawDegeneratesQuadranglestripsAsPoints() const;
  void DrawDegeneratesQuadranglestripsAsLines (const float theSkipRatio) const;
  void DrawDegeneratesPolygonsAsPoints() const;
  void DrawDegeneratesPolygonsAsLines (const float theSkipRatio) const;

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
