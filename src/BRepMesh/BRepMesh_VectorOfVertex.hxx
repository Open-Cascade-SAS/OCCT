// File:        BRepMesh_VectorOfVertex.hxx
// Created:     Jun 1 18:11:53 2011
// Author:      Oleg AGASHIN
// Copyright:   Open CASCADE SAS 2011


#ifndef _BRepMesh_VectorOfVertex_HeaderFile
#define _BRepMesh_VectorOfVertex_HeaderFile

#ifndef _BRepMesh_Vertex_HeaderFile
#include <BRepMesh_Vertex.hxx>
#endif
#ifndef NCollection_Vector_HeaderFile
#include <NCollection_Vector.hxx>
#endif

typedef NCollection_Vector<BRepMesh_Vertex> BRepMesh_VectorOfVertex;

#endif
