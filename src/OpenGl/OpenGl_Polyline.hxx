// File:      OpenGl_Polyline.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Polyline_Header
#define OpenGl_Polyline_Header

#include <InterfaceGraphic_telem.hxx>

#include <OpenGl_Element.hxx>

#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>

class OpenGl_Polyline : public OpenGl_Element
{
 public:

  OpenGl_Polyline (const Graphic3d_Array1OfVertex& AListVertex);
  OpenGl_Polyline (const Graphic3d_Array1OfVertexC& AListVertex);
  OpenGl_Polyline (const Standard_Real X1,const Standard_Real Y1,const Standard_Real Z1,
                  const Standard_Real X2,const Standard_Real Y2,const Standard_Real Z2);
  virtual ~OpenGl_Polyline ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  Tint       myNbVertices;    // Number of vertices in vertices array
  tel_point  myVertices;      // Vertices array of length myNbVertices
  tel_colour myColors;        // Vertex color values for each vertex

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_Polyline_Header
