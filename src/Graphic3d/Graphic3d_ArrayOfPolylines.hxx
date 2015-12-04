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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


class Graphic3d_ArrayOfPolylines;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfPolylines, Graphic3d_ArrayOfPrimitives)

//! Contains polylines array definition
class Graphic3d_ArrayOfPolylines : public Graphic3d_ArrayOfPrimitives
{

public:

  
  //! Creates an array of polylines,
  //! a polyline can be filled as:
  //! 1) creating a single polyline defined with his vertexs.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfPolylines(7)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x7,y7,z7)
  //! 2) creating separate polylines defined with a predefined
  //! number of bounds and the number of vertex per bound.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfPolylines(7,2)
  //! myArray->AddBound(4)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x4,y4,z4)
  //! myArray->AddBound(3)
  //! myArray->AddVertex(x5,y5,z5)
  //! ....
  //! myArray->AddVertex(x7,y7,z7)
  //! 3) creating a single indexed polyline defined with his vertex
  //! ans edges.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfPolylines(4,0,6)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x4,y4,z4)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(3)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(4)
  //! 4) creating separate polylines defined with a predefined
  //! number of bounds and the number of edges per bound.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfPolylines(6,4,14)
  //! myArray->AddBound(3)
  //! myArray->AddVertex(x1,y1,z1)
  //! myArray->AddVertex(x2,y2,z2)
  //! myArray->AddVertex(x3,y3,z3)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(3)
  //! myArray->AddBound(3)
  //! myArray->AddVertex(x4,y4,z4)
  //! myArray->AddVertex(x5,y5,z5)
  //! myArray->AddVertex(x6,y6,z6)
  //! myArray->AddEdge(4)
  //! myArray->AddEdge(5)
  //! myArray->AddEdge(6)
  //! myArray->AddBound(4)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(3)
  //! myArray->AddEdge(5)
  //! myArray->AddEdge(6)
  //! myArray->AddBound(4)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(3)
  //! myArray->AddEdge(5)
  //! myArray->AddEdge(4)
  //!
  //! <maxVertexs> defined the maximun allowed vertex number in the array.
  //! <maxBounds> defined the maximun allowed bound number in the array.
  //! <maxEdges> defined the maximun allowed edge number in the array.
  //! Warning:
  //! When <hasVColors> is TRUE , you must use one of
  //! AddVertex(Point,Color)
  //! or  AddVertex(Point,Normal,Color) methods.
  //! When <hasBColors> is TRUE , <maxBounds> must be > 0 and
  //! you must use the
  //! AddBound(number,Color) method.
  Standard_EXPORT Graphic3d_ArrayOfPolylines(const Standard_Integer maxVertexs, const Standard_Integer maxBounds = 0, const Standard_Integer maxEdges = 0, const Standard_Boolean hasVColors = Standard_False, const Standard_Boolean hasBColors = Standard_False);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfPolylines,Graphic3d_ArrayOfPrimitives)

protected:




private:




};







#endif // _Graphic3d_ArrayOfPolylines_HeaderFile
