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

#ifndef _Graphic3d_ArrayOfPoints_HeaderFile
#define _Graphic3d_ArrayOfPoints_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>

//! Contains points array definition.
class Graphic3d_ArrayOfPoints : public Graphic3d_ArrayOfPrimitives
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPoints, Graphic3d_ArrayOfPrimitives)
public:
  //! Creates an array of points (Graphic3d_TOPA_POINTS).
  //! The array must be filled using the AddVertex(Point) method.
  //! @param theMaxVertexs maximum number of points
  //! @param theArrayFlags array flags
  Graphic3d_ArrayOfPoints(Standard_Integer theMaxVertexs, Graphic3d_ArrayFlags theArrayFlags)
      : Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_POINTS, theMaxVertexs, 0, 0, theArrayFlags)
  {
  }

  //! Creates an array of points (Graphic3d_TOPA_POINTS).
  //! The array must be filled using the AddVertex(Point) method.
  //! @param theMaxVertexs  maximum number of points
  //! @param theHasVColors  when TRUE, AddVertex(Point,Color)  should be used for specifying vertex
  //! color
  //! @param theHasVNormals when TRUE, AddVertex(Point,Normal) should be used for specifying vertex
  //! normal
  Graphic3d_ArrayOfPoints(Standard_Integer theMaxVertexs,
                          Standard_Boolean theHasVColors  = Standard_False,
                          Standard_Boolean theHasVNormals = Standard_False)
      : Graphic3d_ArrayOfPrimitives(
          Graphic3d_TOPA_POINTS,
          theMaxVertexs,
          0,
          0,
          (theHasVColors ? Graphic3d_ArrayFlags_VertexColor : Graphic3d_ArrayFlags_None)
            | (theHasVNormals ? Graphic3d_ArrayFlags_VertexNormal : Graphic3d_ArrayFlags_None))
  {
  }
};

DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPoints, Graphic3d_ArrayOfPrimitives)

#endif // _Graphic3d_ArrayOfPoints_HeaderFile
