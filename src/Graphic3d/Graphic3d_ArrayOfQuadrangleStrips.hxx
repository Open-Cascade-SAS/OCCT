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

#ifndef _Graphic3d_ArrayOfQuadrangleStrips_HeaderFile
#define _Graphic3d_ArrayOfQuadrangleStrips_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>

//! Contains quadrangles strip array definition.
//! WARNING! Quadrangle primitives might be unsupported by graphics library.
//! Triangulation should be used instead of quads for better compatibility.
class Graphic3d_ArrayOfQuadrangleStrips : public Graphic3d_ArrayOfPrimitives
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfQuadrangleStrips, Graphic3d_ArrayOfPrimitives)
public:

  //! Creates an array of quadrangle strips, a polygon can be filled as:
  //! 1) Creating a single strip defined with his vertexes, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfQuadrangleStrips (7);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x7, y7, z7);
  //! @endcode
  //! 2) Creating separate strips defined with a predefined number of strips and the number of vertex per strip, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfQuadrangleStrips (8, 2);
  //!   myArray->AddBound (4);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x4, y4, z4);
  //!   myArray->AddBound (4);
  //!   myArray->AddVertex (x5, y5, z5);
  //!   ....
  //!   myArray->AddVertex (x8, y8, z8);
  //! @endcode
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxStrips  defines the maximum allowed strip  number in the array
  //! The number of quadrangle really drawn is: VertexNumber()/2 - Min(1, BoundNumber()).
  Graphic3d_ArrayOfQuadrangleStrips (const Standard_Integer theMaxVertexs,
                                     const Standard_Integer theMaxStrips   = 0,
                                     const Standard_Boolean theHasVNormals = Standard_False,
                                     const Standard_Boolean theHasVColors  = Standard_False,
                                     const Standard_Boolean theHasSColors  = Standard_False,
                                     const Standard_Boolean theHasVTexels  = Standard_False)
  : Graphic3d_ArrayOfPrimitives (Graphic3d_TOPA_QUADRANGLESTRIPS, theMaxVertexs, theMaxStrips, 0, theHasVNormals, theHasVColors, theHasSColors, theHasVTexels) {}

};

DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfQuadrangleStrips, Graphic3d_ArrayOfPrimitives)

#endif // _Graphic3d_ArrayOfQuadrangleStrips_HeaderFile
