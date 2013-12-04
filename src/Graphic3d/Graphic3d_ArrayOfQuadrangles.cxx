// Created on: 2001-01-04
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <Graphic3d_ArrayOfQuadrangles.ixx>

Graphic3d_ArrayOfQuadrangles :: Graphic3d_ArrayOfQuadrangles (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasVTexels,
			const Standard_Boolean hasEdgesInfos)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_QUADRANGLES,maxVertexs,0,maxEdges,hasVNormals,hasVColors,Standard_False,hasVTexels,hasEdgesInfos) {}
