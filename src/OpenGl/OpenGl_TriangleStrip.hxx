// File:      OpenGl_TriangleStrip.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_TriangleStrip_Header
#define OpenGl_TriangleStrip_Header

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Array1OfVertexN.hxx>
#include <Graphic3d_Array1OfVertexNT.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_Element.hxx>

struct TEL_TMESH_DATA
{
  Tint       num_facets; /* Number of facets */
  Tint       facet_flag; /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag;/* TEL_VT_NONE or TEL_VT_NORMAL */
  tel_point  fnormals;   /* length = num_facets */
  tel_colour fcolours;   /* length = num_facets */
  tel_point  vertices;   /* length = num_facets + 2 */
  tel_colour vcolours;   /* length = num_facets + 2 */
  tel_point  vnormals;   /* length = num_facets + 2 */
  tel_texture_coord vtexturecoord; /* Texture coordinates */
  IMPLEMENT_MEMORY_OPERATORS
};

class OpenGl_TriangleStrip : public OpenGl_Element
{
 public:

  OpenGl_TriangleStrip (const Graphic3d_Array1OfVertex& AListVertex);
  OpenGl_TriangleStrip (const Graphic3d_Array1OfVertexN& AListVertex);
  OpenGl_TriangleStrip (const Graphic3d_Array1OfVertexNT& AListVertex);
  virtual ~OpenGl_TriangleStrip ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  void Init (const Tint ANbFacets, tel_point AVertices,
             tel_point AVNormals, tel_colour AVColors, tel_texture_coord ATCoords,
             tel_point AFNormals, tel_colour AFColors);

  void draw_tmesh (const Tint,          /* front_lighting_model, */
                   const Aspect_InteriorStyle, /* interior_style */
                   const TEL_COLOUR *, /* edge_colour */
                   const Handle(OpenGl_Workspace) &) const;

  void draw_edges (const TEL_COLOUR *, const Aspect_InteriorStyle, Tint, const Handle(OpenGl_Workspace) &) const;

  void draw_line_loop () const;

  TEL_TMESH_DATA myData;
  DS_INTERNAL *myDS;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_TriangleStrip_Header
