// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef _InterfaceGraphic_PrimitiveArray_header_file_
#define _InterfaceGraphic_PrimitiveArray_header_file_

/*
FILE: InterfaceGraphic_PrimitiveArray.hxx

Created 16/06/2000 : ATS,SPK : G005

This file contains definitios of internal structures for 
PARRAY and DARRAY primitives, used in OpenGl package for presentation

*/

#include <InterfaceGraphic_telem.hxx>
#include <InterfaceGraphic_degeneration.hxx>

#define MVERTICE 1
#define MVNORMAL 2
#define MVCOLOR 4
#define MVTEXEL 8

typedef enum {
  TelUnknownArrayType,
  TelPointsArrayType,
  TelPolylinesArrayType,
  TelSegmentsArrayType,
  TelPolygonsArrayType,
  TelTrianglesArrayType,
  TelQuadranglesArrayType,
  TelTriangleStripsArrayType,
  TelQuadrangleStripsArrayType,
  TelTriangleFansArrayType
} TelPrimitivesArrayType;

typedef struct {
  TelPrimitivesArrayType type;                    /* Array type */
  Tint                   format;                  /* Array datas format */
  Tint                   num_vertexs;             /* Number of vertexs */
  Tint                   num_bounds;              /* Number of bounds */
  Tint                   num_edges;               /* Number of edges */
  Tint                  *bounds;                  /* Bounds array */
  Tint                  *edges;                   /* Edges array vertex index */
  tel_colour             fcolours;                /* Facet colour values */
  tel_point              vertices;                /* Vertices */
  Tint                  *vcolours;                /* Vertex colour values */
  tel_point              vnormals;                /* Vertex normals */
  tel_texture_coord      vtexels;                 /* Texture Coordinates */
  Tchar                 *edge_vis;                /* Edge visibility flag*/
  Tchar                 *keys;                    /* Vertex keys*/
} CALL_DEF_PARRAY;

#endif /* _InterfaceGraphic_PrimitiveArray_header_file_ */
