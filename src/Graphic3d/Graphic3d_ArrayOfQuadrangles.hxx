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

#ifndef _Graphic3d_ArrayOfQuadrangles_HeaderFile
#define _Graphic3d_ArrayOfQuadrangles_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>

//! Contains quadrangles array definition.
//! WARNING! Quadrangle primitives might be unsupported by graphics library.
//! Triangulation should be used instead of quads for better compatibility.
class Graphic3d_ArrayOfQuadrangles : public Graphic3d_ArrayOfPrimitives
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfQuadrangles, Graphic3d_ArrayOfPrimitives)
public:

  //! Creates an array of quadrangles (Graphic3d_TOPA_QUADRANGLES), a quadrangle can be filled as:
  //! 1) Creating a set of quadrangles defined with his vertexes, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfQuadrangles (8);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x8, y8, z8);
  //! @endcode
  //! 2) Creating a set of indexed quadrangles defined with his vertex ans edges, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfQuadrangles (6, 8);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x6, y6, z6);
  //!   myArray->AddEdges (1, 2, 3, 4);
  //!   myArray->AddEdges (3, 4, 5, 6);
  //! @endcode
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxEdges   defines the maximum allowed edge   number in the array (for indexed array)
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfQuadrangles (Standard_Integer theMaxVertexs,
                                Standard_Integer theMaxEdges,
                                Graphic3d_ArrayFlags theArrayFlags)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_QUADRANGLES, theMaxVertexs, 0, theMaxEdges, theArrayFlags) {}

  //! Creates an array of quadrangles (Graphic3d_TOPA_QUADRANGLES).
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxEdges   defines the maximum allowed edge   number in the array (for indexed array)
  Graphic3d_ArrayOfQuadrangles (Standard_Integer theMaxVertexs,
                                Standard_Integer theMaxEdges    = 0,
                                Standard_Boolean theHasVNormals = Standard_False,
                                Standard_Boolean theHasVColors  = Standard_False,
                                Standard_Boolean theHasVTexels  = Standard_False)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_QUADRANGLES, theMaxVertexs, 0, theMaxEdges,
                                 (theHasVNormals ? Graphic3d_ArrayFlags_VertexNormal : Graphic3d_ArrayFlags_None)
                               | (theHasVColors  ? Graphic3d_ArrayFlags_VertexColor  : Graphic3d_ArrayFlags_None)
                               | (theHasVTexels  ? Graphic3d_ArrayFlags_VertexTexel  : Graphic3d_ArrayFlags_None)) {}

};

DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfQuadrangles, Graphic3d_ArrayOfPrimitives)

#endif // _Graphic3d_ArrayOfQuadrangles_HeaderFile
