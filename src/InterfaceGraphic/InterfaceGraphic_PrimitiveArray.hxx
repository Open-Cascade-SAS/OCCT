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

typedef enum 
{
  VBOEdges, 
  VBOVertices, 
  VBOVcolours, 
  VBOVnormals, 
  VBOVtexels,
  VBOMaxType
} VBODataType;

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
  Tuint                  bufferVBO[VBOMaxType];   /* VBO IDs for *edges, vertices, vcolours, vnormals, vtexels.
                                                     default = -1. VBO - Vertex Buffer Object */
  Tint                   flagBufferVBO;           /* this flag is responded for bufferVBO load status.
                                                     -1 - Not Initial BufferVBO. Default mean.
                                                      0 - Error by allocated memory in Graphic Device.
                                                      Not Initial BufferVBO.
                                                      1 - Initial BufferVBO */
  Tint                   VBOEnabled;              /* -1 - it is not known VBO status
                                                      0 - draw object without VBO
                                                      1 - draw object with VBO */
  Standard_Address       contextId;               /* remember context for VBO */
} CALL_DEF_PARRAY;

#endif /* _InterfaceGraphic_PrimitiveArray_header_file_ */
