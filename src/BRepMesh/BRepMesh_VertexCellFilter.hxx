// Created on: 2011-06-02
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
