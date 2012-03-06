// File:      OpenGl_QuadrangleStrip.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_QuadrangleStrip_Header
#define OpenGl_QuadrangleStrip_Header

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_Array2OfVertex.hxx>
#include <Graphic3d_Array2OfVertexN.hxx>
#include <Graphic3d_Array2OfVertexNT.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_Element.hxx>

struct OPENGL_QSTRIP_DATA
{
  Tint       shape_flag;  /* TEL_SHAPE_UNKNOWN or TEL_SHAPE_COMPLEX or
                          TEL_SHAPE_CONVEX  or TEL_SHAPE_CONCAVE */
  Tint       facet_flag;  /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag; /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       num_rows, num_columns; /* Number of rows and columns */
  tel_point  fnormals;    /* Facet normals */
  tel_colour fcolours;    /* Facet colours */
  tel_point  vertices;    /* Vertices */
  tel_colour vcolours;    /* Vertex colour values */
  tel_point  vnormals;    /* Vertex normals */
  tel_texture_coord vtexturecoord; /* Texture coordinates */
  DEFINE_STANDARD_ALLOC
};

class OpenGl_QuadrangleStrip : public OpenGl_Element
{
 public:

  OpenGl_QuadrangleStrip (const Graphic3d_Array2OfVertex& AListVertex);
  OpenGl_QuadrangleStrip (const Graphic3d_Array2OfVertexN& AListVertex);
  OpenGl_QuadrangleStrip (const Graphic3d_Array2OfVertexNT& AListVertex);
  virtual ~OpenGl_QuadrangleStrip ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  void Init (const Tint ANbRows, const Tint ANbCols, tel_point AVertices,
             tel_point AVNormals, tel_colour AVColors, tel_texture_coord ATCoords,
             tel_point AFNormals, tel_colour AFColors, const Tint AShapeFlag);

  void draw_qstrip (const Tint,          /* front_lighting_model,  */
                    const Aspect_InteriorStyle, /* interior_style, */
                    const TEL_COLOUR *, /* edge_colour, */
                    const Handle(OpenGl_Workspace) &) const;

  void draw_edges (const TEL_COLOUR *, const Aspect_InteriorStyle, Tint, const Handle(OpenGl_Workspace) &) const;

  void draw_line_strip (const TEL_COLOUR *, Tint, Tint) const;

  OPENGL_QSTRIP_DATA myData;
  DS_INTERNAL *myDS;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_QuadrangleStrip_Header
