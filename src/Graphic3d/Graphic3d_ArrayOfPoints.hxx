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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


class Graphic3d_ArrayOfPoints;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPoints, Graphic3d_ArrayOfPrimitives)

//! Contains points array definition
class Graphic3d_ArrayOfPoints : public Graphic3d_ArrayOfPrimitives
{

public:

  
  //! Creates an array of points,
  //! a single pixel point is drawn at each vertex.
  //! The array must be filled using the AddVertex(Point) method.
  //! When <hasVColors> is TRUE , you must use only AddVertex(Point,Color) method.
  //! When <hasVNormals> is TRUE , you must use only AddVertex(Point,Normal) method.
  Standard_EXPORT Graphic3d_ArrayOfPoints(const Standard_Integer maxVertexs, const Standard_Boolean hasVColors = Standard_False, const Standard_Boolean hasVNormals = Standard_False);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPoints,Graphic3d_ArrayOfPrimitives)

protected:




private:




};







#endif // _Graphic3d_ArrayOfPoints_HeaderFile
