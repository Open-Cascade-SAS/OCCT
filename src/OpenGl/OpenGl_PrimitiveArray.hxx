// File:      OpenGl_PrimitiveArray.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_PrimitiveArray_Header
#define OpenGl_PrimitiveArray_Header

#include <OpenGl_Context.hxx>

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_Element.hxx>

struct OPENGL_SURF_PROP;

class OpenGl_PrimitiveArray : public OpenGl_Element
{
public:

  //! Default constructor
  OpenGl_PrimitiveArray (CALL_DEF_PARRAY* thePArray);

  //! Destructor
  virtual ~OpenGl_PrimitiveArray();

  //! Render primitives to the window
  virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const;

  CALL_DEF_PARRAY* PArray() const { return myPArray; }

private:

  //! VBO initialization procedures
  Standard_Boolean BuildVBO (const Handle(OpenGl_Workspace)& theWorkspace) const;
  void clearMemoryOwn() const;
  void clearMemoryGL (const Handle(OpenGl_Context)& theGlContext) const;
  Standard_Boolean checkSizeForGraphicMemory (const Handle(OpenGl_Context)& theGlContext) const;

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

  mutable CALL_DEF_PARRAY* myPArray;
  GLenum                   myDrawMode;

public:

  IMPLEMENT_MEMORY_OPERATORS

};

#endif //OpenGl_PrimitiveArray_Header
