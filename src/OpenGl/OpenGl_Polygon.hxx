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


#ifndef OpenGl_Polygon_Header
#define OpenGl_Polygon_Header

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_TypeOfPolygon.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_Element.hxx>

#include <NCollection_Vector.hxx>

struct SEQ_;

struct TEL_POLYGON_DATA
{
  Tint       num_vertices;  /* Number of vertices */
  Tint       facet_flag;  /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag; /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       shape_flag;  /* TEL_SHAPE_UNKNOWN or TEL_SHAPE_COMPLEX or
                          TEL_SHAPE_CONVEX  or TEL_SHAPE_CONCAVE */
  Tint reverse_order; /* 1 if polygon vertex must be display in reverse order */
  TEL_POINT  fnormal;   /* Facet normal */
  tel_colour fcolour;   /* Facet colour */
  tel_point  vertices;    /* Vertices */
  tel_colour vcolours;    /* Vertex colour values */
  tel_point  vnormals;    /* Vertex normals */
  tel_texture_coord vtexturecoord; /* Texture Coordinates */
  NCollection_Vector<SEQ_> *dsply;
  DEFINE_STANDARD_ALLOC
};

class OpenGl_Polygon : public OpenGl_Element
{

public:

  OpenGl_Polygon (const Graphic3d_Array1OfVertex& AListVertex,
                 const Graphic3d_TypeOfPolygon AType);

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

protected:

  virtual ~OpenGl_Polygon();

  void draw_polygon_concav (const Handle(OpenGl_Workspace) &AWorkspace, Tint) const;

  void draw_polygon (const Handle(OpenGl_Workspace) &AWorkspace, Tint) const;

  void draw_tmesh (Tint) const;

  void draw_edges (const TEL_COLOUR *, const Aspect_InteriorStyle, const Handle(OpenGl_Workspace) &) const;

  TEL_POLYGON_DATA myData;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Polygon_Header
