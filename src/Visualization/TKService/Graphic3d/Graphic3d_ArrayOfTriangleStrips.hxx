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

#ifndef _Graphic3d_ArrayOfTriangleStrips_HeaderFile
#define _Graphic3d_ArrayOfTriangleStrips_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>

//! Contains triangles strip array definition.
class Graphic3d_ArrayOfTriangleStrips : public Graphic3d_ArrayOfPrimitives
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfTriangleStrips, Graphic3d_ArrayOfPrimitives)
public:
  //! Creates an array of triangle strips (Graphic3d_TOPA_TRIANGLESTRIPS), a polygon can be filled
  //! as: 1) Creating a single strip defined with his vertexes, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfTriangleStrips (7);
  //!   myArray->AddVertex (x1, y1, z1);
  //!   ....
  //!   myArray->AddVertex (x7, y7, z7);
  //! @endcode
  //! 2) Creating separate strips defined with a predefined number of strips and the number of
  //! vertex per strip, i.e:
  //! @code
  //!   myArray = Graphic3d_ArrayOfTriangleStrips (8, 2);
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
  //! @param theMaxStrips  defines the maximum allowed strip  number in the array;
  //!                      the number of triangle really drawn is: VertexNumber() - 2 * Min(1,
  //!                      BoundNumber())
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfTriangleStrips(Standard_Integer     theMaxVertexs,
                                  Standard_Integer     theMaxStrips,
                                  Graphic3d_ArrayFlags theArrayFlags)
      : Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_TRIANGLESTRIPS,
                                    theMaxVertexs,
                                    theMaxStrips,
                                    0,
                                    theArrayFlags)
  {
  }

  //! Creates an array of triangle strips (Graphic3d_TOPA_TRIANGLESTRIPS).
  //! @param theMaxVertexs defines the maximum allowed vertex number in the array
  //! @param theMaxStrips  defines the maximum allowed strip  number in the array;
  //!                      the number of triangle really drawn is: VertexNumber() - 2 * Min(1,
  //!                      BoundNumber())
  //! @param theHasVNormals when TRUE, AddVertex(Point,Normal), AddVertex(Point,Normal,Color) or
  //! AddVertex(Point,Normal,Texel) should be used to specify vertex normal;
  //!                       vertex normals should be specified coherent to triangle orientation
  //!                       (defined by order of vertexes within triangle) for proper rendering
  //! @param theHasVColors  when TRUE, AddVertex(Point,Color) or AddVertex(Point,Normal,Color)
  //! should be used to specify vertex color
  //! @param theHasBColors  when TRUE, AddBound(number,Color) should be used to specify sub-group
  //! color
  //! @param theHasVTexels  when TRUE, AddVertex(Point,Texel) or AddVertex(Point,Normal,Texel)
  //! should be used to specify vertex UV coordinates
  Graphic3d_ArrayOfTriangleStrips(Standard_Integer theMaxVertexs,
                                  Standard_Integer theMaxStrips   = 0,
                                  Standard_Boolean theHasVNormals = Standard_False,
                                  Standard_Boolean theHasVColors  = Standard_False,
                                  Standard_Boolean theHasBColors  = Standard_False,
                                  Standard_Boolean theHasVTexels  = Standard_False)
      : Graphic3d_ArrayOfPrimitives(
          Graphic3d_TOPA_TRIANGLESTRIPS,
          theMaxVertexs,
          theMaxStrips,
          0,
          (theHasVNormals ? Graphic3d_ArrayFlags_VertexNormal : Graphic3d_ArrayFlags_None)
            | (theHasVColors ? Graphic3d_ArrayFlags_VertexColor : Graphic3d_ArrayFlags_None)
            | (theHasVTexels ? Graphic3d_ArrayFlags_VertexTexel : Graphic3d_ArrayFlags_None)
            | (theHasBColors ? Graphic3d_ArrayFlags_BoundColor : Graphic3d_ArrayFlags_None))
  {
  }
};

DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfTriangleStrips, Graphic3d_ArrayOfPrimitives)

#endif // _Graphic3d_ArrayOfTriangleStrips_HeaderFile
