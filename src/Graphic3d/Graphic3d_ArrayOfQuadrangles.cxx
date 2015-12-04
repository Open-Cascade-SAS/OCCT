// Created on: 2001-01-04
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ArrayOfQuadrangles,Graphic3d_ArrayOfPrimitives)

Graphic3d_ArrayOfQuadrangles::Graphic3d_ArrayOfQuadrangles (const Standard_Integer theMaxVertexs,
                                                            const Standard_Integer theMaxEdges,
                                                            const Standard_Boolean theHasVNormals,
                                                            const Standard_Boolean theHasVColors,
                                                            const Standard_Boolean theHasVTexels)
: Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_QUADRANGLES, theMaxVertexs, 0, theMaxEdges, theHasVNormals, theHasVColors, Standard_False, theHasVTexels)
{}
