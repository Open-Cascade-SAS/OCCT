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

#ifndef _Graphic3d_ArrayOfSegments_HeaderFile
#define _Graphic3d_ArrayOfSegments_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>

//! Contains segments array definition.
class Graphic3d_ArrayOfSegments : public Graphic3d_ArrayOfPrimitives
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfSegments, Graphic3d_ArrayOfPrimitives)
public:

  //! Creates an array of segments (Graphic3d_TOPA_SEGMENTS), a segment can be filled as:
  //! 1) Creating a set of segments defined with his vertexes, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfSegments (4);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x4, y4, z4);
  //! @endcode
  //! 2) Creating a set of indexed segments defined with his vertex and edges, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfSegments (4, 8);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x4, y4, z4);
  //!   myArray->AddEdges (1, 2);
  //!   myArray->AddEdges (3, 4);
  //!   myArray->AddEdges (2, 4);
  //!   myArray->AddEdges (1, 3);
  //! @endcode
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxEdges   defines the maximum allowed edge   number in the array
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfSegments (Standard_Integer theMaxVertexs,
                             Standard_Integer theMaxEdges,
                             Graphic3d_ArrayFlags theArrayFlags)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_SEGMENTS, theMaxVertexs, 0, theMaxEdges, theArrayFlags) {}

  //! Creates an array of segments (Graphic3d_TOPA_SEGMENTS).
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxEdges   defines the maximum allowed edge   number in the array
  //! @param theHasVColors when TRUE, AddVertex(Point,Color) should be used for specifying vertex color
  Graphic3d_ArrayOfSegments (Standard_Integer theMaxVertexs,
                             Standard_Integer theMaxEdges   = 0,
                             Standard_Boolean theHasVColors = Standard_False)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_SEGMENTS, theMaxVertexs, 0, theMaxEdges, theHasVColors ? Graphic3d_ArrayFlags_VertexColor : Graphic3d_ArrayFlags_None) {}


};

DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfSegments, Graphic3d_ArrayOfPrimitives)

#endif // _Graphic3d_ArrayOfSegments_HeaderFile
