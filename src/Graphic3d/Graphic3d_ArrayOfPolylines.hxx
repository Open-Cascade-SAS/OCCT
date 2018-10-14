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

#ifndef _Graphic3d_ArrayOfPolylines_HeaderFile
#define _Graphic3d_ArrayOfPolylines_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>

//! Contains polylines array definition.
class Graphic3d_ArrayOfPolylines : public Graphic3d_ArrayOfPrimitives
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPolylines, Graphic3d_ArrayOfPrimitives)
public:

  //! Creates an array of polylines (Graphic3d_TOPA_POLYLINES), a polyline can be filled as:
  //! 1) Creating a single polyline defined with his vertexes, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfPolylines (7);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x7, y7, z7);
  //! @endcode
  //! 2) Creating separate polylines defined with a predefined number of bounds and the number of vertex per bound, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfPolylines (7, 2);
  //!   myArray->AddBound (4);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x4, y4, z4);
  //!   myArray->AddBound (3);
  //!   myArray->AddVertex (x5, y5, z5);
  //!   ....
  //!   myArray->AddVertex (x7, y7, z7);
  //! @endcode
  //! 3) Creating a single indexed polyline defined with his vertex and edges, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfPolylines (4, 0, 6);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x4, y4, z4);
  //!   myArray->AddEdge (1);
  //!   myArray->AddEdge (2);
  //!   myArray->AddEdge (3);
  //!   myArray->AddEdge (1);
  //!   myArray->AddEdge (2);
  //!   myArray->AddEdge (4);
  //! @endcode
  //! 4) creating separate polylines defined with a predefined number of bounds and the number of edges per bound, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfPolylines (6, 4, 14);
  //!   myArray->AddBound (3);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   myArray->AddVertex (x2, y2, z2);
  //!   myArray->AddVertex (x3, y3, z3);
  //!   myArray->AddEdge (1);
  //!   myArray->AddEdge (2);
  //!   myArray->AddEdge (3);
  //!   myArray->AddBound (3);
  //!   myArray->AddVertex (x4, y4, z4);
  //!   myArray->AddVertex (x5, y5, z5);
  //!   myArray->AddVertex (x6, y6, z6);
  //!   myArray->AddEdge (4);
  //!   myArray->AddEdge (5);
  //!   myArray->AddEdge (6);
  //!   myArray->AddBound (4);
  //!   myArray->AddEdge (2);
  //!   myArray->AddEdge (3);
  //!   myArray->AddEdge (5);
  //!   myArray->AddEdge (6);
  //!   myArray->AddBound (4);
  //!   myArray->AddEdge (1);
  //!   myArray->AddEdge (3);
  //!   myArray->AddEdge (5);
  //!   myArray->AddEdge (4);
  //! @endcode
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxBounds  defines the maximum allowed bound  number in the array
  //! @param theMaxEdges   defines the maximum allowed edge   number in the array
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfPolylines (Standard_Integer theMaxVertexs,
                              Standard_Integer theMaxBounds,
                              Standard_Integer theMaxEdges,
                              Graphic3d_ArrayFlags theArrayFlags)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_POLYLINES, theMaxVertexs, theMaxBounds, theMaxEdges, theArrayFlags) {}

  //! Creates an array of polylines (Graphic3d_TOPA_POLYLINES).
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxBounds  defines the maximum allowed bound  number in the array
  //! @param theMaxEdges   defines the maximum allowed edge   number in the array
  //! @param theHasVColors when TRUE AddVertex(Point,Color) or AddVertex(Point,Normal,Color) should be used to specify per-vertex color values
  //! @param theHasBColors when TRUE AddBound(number,Color) should be used to specify sub-group color
  Graphic3d_ArrayOfPolylines (Standard_Integer theMaxVertexs,
                              Standard_Integer theMaxBounds  = 0,
                              Standard_Integer theMaxEdges   = 0,
                              Standard_Boolean theHasVColors = Standard_False,
                              Standard_Boolean theHasBColors = Standard_False)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_POLYLINES, theMaxVertexs, theMaxBounds, theMaxEdges,
                                 (theHasVColors  ? Graphic3d_ArrayFlags_VertexColor  : Graphic3d_ArrayFlags_None)
                               | (theHasBColors  ? Graphic3d_ArrayFlags_BoundColor   : Graphic3d_ArrayFlags_None)) {}

};

DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPolylines, Graphic3d_ArrayOfPrimitives)

#endif // _Graphic3d_ArrayOfPolylines_HeaderFile
