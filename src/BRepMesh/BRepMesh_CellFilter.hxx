// File:        BRepMesh_CellFilter.hxx
// Created:     May 26 16:40:53 2008
// Author:      Ekaterina SMIRNOVA
// Copyright:   Open CASCADE SAS 2008


#include <gp_XYZ.hxx>
#include <gp_XY.hxx>
#include <NCollection_CellFilter.hxx>
#include <BRepMesh_CircleInspector.hxx>

typedef NCollection_CellFilter<BRepMesh_CircleInspector> BRepMesh_CellFilter;
