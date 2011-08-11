// File:        BRepMesh_VertexCellFilter.hxx
// Created:     Jun 2 10:17:51 2011
// Author:      Oleg AGASHIN
// Copyright:   Open CASCADE SAS 2011

#ifndef _BRepMesh_VertexCellFilter_HeaderFile
#define _BRepMesh_VertexCellFilter_HeaderFile

#ifndef _gp_XYZ_HeaderFile
#include <gp_XYZ.hxx>
#endif
#ifndef _gp_XY_HeaderFile
#include <gp_XY.hxx>
#endif
#ifndef NCollection_CellFilter_HeaderFile
#include <NCollection_CellFilter.hxx>
#endif
#ifndef _BRepMesh_VertexInspector_HeaderFile
#include <BRepMesh_VertexInspector.hxx>
#endif

typedef NCollection_CellFilter<BRepMesh_VertexInspector> BRepMesh_VertexCellFilter;

#endif
