// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _InterfaceGraphic_PrimitiveArray_header_file_
#define _InterfaceGraphic_PrimitiveArray_header_file_

/*
FILE: InterfaceGraphic_PrimitiveArray.hxx

Created 16/06/2000 : ATS,SPK : G005

This file contains definitios of internal structures for 
PARRAY and DARRAY primitives, used in OpenGl package for presentation

*/

#include <InterfaceGraphic_telem.hxx>

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
