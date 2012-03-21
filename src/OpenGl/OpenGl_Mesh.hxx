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


#ifndef OpenGl_Mesh_Header
#define OpenGl_Mesh_Header

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Array1OfVertexN.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>
#include <Graphic3d_Array1OfVertexNC.hxx>
#include <Graphic3d_Array1OfVertexNT.hxx>
#include <Aspect_Array1OfEdge.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_Element.hxx>
#include <OpenGl_AspectFace.hxx>

struct TEL_INDEXPOLY_DATA
{
  Tint       num_vertices;   /* Number of vertices */
  Tint       num_facets;     /* Number of facets (triangles, quadrangles or polygons) */
  Tint       num_bounds;     /* Number of bounds in a facet (3, 4 or more) */
  Tint       facet_flag;     /* TEL_FA_NONE or TEL_FA_NORMAL */
  Tint       vertex_flag;    /* TEL_VT_NONE or TEL_VT_NORMAL */
  Tint       *edge_vis;      /* Edge visibility indicators for each edge */
  Tint       *indices;       /* Connectivity array */
  tel_point  fnormals;       /* Facet normals */
  tel_colour fcolours;       /* Facet colour values */
  tel_point  vertices;       /* Vertices */
  tel_colour vcolours;       /* Vertex colour values */
  tel_point  vnormals;       /* Vertex normals */
  tel_texture_coord vtexturecoord; /* Texture Coordinates */
  DEFINE_STANDARD_ALLOC
};

class OpenGl_Mesh : public OpenGl_Element
{
 public:

  OpenGl_Mesh (const Graphic3d_Array1OfVertex& AListVertex, const Aspect_Array1OfEdge& AListEdge);
  OpenGl_Mesh (const Graphic3d_Array1OfVertexN& AListVertex, const Aspect_Array1OfEdge& AListEdge);
  OpenGl_Mesh (const Graphic3d_Array1OfVertexC& AListVertex, const Aspect_Array1OfEdge& AListEdge);
  OpenGl_Mesh (const Graphic3d_Array1OfVertexNC& AListVertex, const Aspect_Array1OfEdge& AListEdge);
  OpenGl_Mesh (const Graphic3d_Array1OfVertexNT& AListVertex, const Aspect_Array1OfEdge& AListEdge);

  virtual ~OpenGl_Mesh ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  void Init (const Tint ANbVertices, tel_point AVertices,
             tel_point AVNormals, tel_colour AVColors, tel_texture_coord ATCoords,
             const Aspect_Array1OfEdge& AListEdge, const Tint ANbBounds);

  void draw_indexpoly (const Tint,          /* front_lighting_model,  */
                       const Aspect_InteriorStyle, /* interior_style, */
                       const TEL_COLOUR *, /* edge_colour, */
                       const OPENGL_SURF_PROP *,
                       const Handle(OpenGl_Workspace) &) const;

  void draw_degenerates_as_points (const float) const;

  void draw_degenerates_as_bboxs () const;

  void draw_edges (const TEL_COLOUR *, const Aspect_InteriorStyle, Tint, const Handle(OpenGl_Workspace) &) const;

  TEL_INDEXPOLY_DATA myData;
  DS_INTERNAL *myDS;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_Mesh_Header
