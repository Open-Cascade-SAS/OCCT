// File:        BRepBuilderAPI_CellFilter.hxx
// Created:     Nov 24 16:48:12 2011
// Author:      ANNA MASALSKAYA
// Copyright:   Open CASCADE SAS 2011

#ifndef _BRepBuilderAPI_CellFilter_HeaderFile
#define _BRepBuilderAPI_CellFilter_HeaderFile

#ifndef _gp_XYZ_HeaderFile
#include <gp_XYZ.hxx>
#endif
#ifndef _gp_XY_HeaderFile
#include <gp_XY.hxx>
#endif
#ifndef NCollection_CellFilter_HeaderFile
#include <NCollection_CellFilter.hxx>
#endif
#ifndef _BRepBuilderAPI_VertexInspector_HeaderFile
#include <BRepBuilderAPI_VertexInspector.hxx>
#endif

typedef NCollection_CellFilter<BRepBuilderAPI_VertexInspector> BRepBuilderAPI_CellFilter;

#endif
