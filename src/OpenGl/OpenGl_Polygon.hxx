// File:      OpenGl_Polygon.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Polygon_Header
#define OpenGl_Polygon_Header

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_TypeOfPolygon.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_Element.hxx>

struct OPENGL_DISPLAY_PGN;

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
  OPENGL_DISPLAY_PGN *dsply;
  DEFINE_STANDARD_ALLOC
};

class OpenGl_Polygon : public OpenGl_Element
{
 public:

  OpenGl_Polygon (const Graphic3d_Array1OfVertex& AListVertex,
                 const Graphic3d_TypeOfPolygon AType);
  virtual ~OpenGl_Polygon ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  void draw_polygon_concav (const Handle(OpenGl_Workspace) &AWorkspace, Tint) const;

  void draw_polygon (const Handle(OpenGl_Workspace) &AWorkspace, Tint) const;

  void draw_tmesh (Tint) const;

  void draw_edges (const TEL_COLOUR *, const Aspect_InteriorStyle, const Handle(OpenGl_Workspace) &) const;

  TEL_POLYGON_DATA myData;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_Polygon_Header
