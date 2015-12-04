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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_OutOfRange;


class Graphic3d_ArrayOfQuadrangles;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfQuadrangles, Graphic3d_ArrayOfPrimitives)

//! Contains quatrangles array definition
class Graphic3d_ArrayOfQuadrangles : public Graphic3d_ArrayOfPrimitives
{

public:

  
  //! Creates an array of quadrangles,
  //! a quadrangle can be filled as:
  //! 1) creating a set of quadrangles defined with his vertexs.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfQuadrangles(8)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x8,y8,z8)
  //! 3) creating a set of indexed quadrangles defined with his vertex
  //! ans edges.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfQuadrangles(6,8)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x6,y6,z6)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(3)
  //! myArray->AddEdge(4)
  //! myArray->AddEdge(3)
  //! myArray->AddEdge(4)
  //! myArray->AddEdge(5)
  //! myArray->AddEdge(6)
  //!
  //! <maxVertexs> defined the maximun allowed vertex number in the array.
  //! <maxEdges> defined the maximun allowed edge number in the array.
  //! Warning:
  //! When <hasVNormals> is TRUE , you must use one of
  //! AddVertex(Point,Normal)
  //! or  AddVertex(Point,Normal,Color)
  //! or  AddVertex(Point,Normal,Texel) methods.
  //! When <hasVColors> is TRUE , you must use one of
  //! AddVertex(Point,Color)
  //! or  AddVertex(Point,Normal,Color) methods.
  //! When <hasTexels> is TRUE , you must use one of
  //! AddVertex(Point,Texel)
  //! or  AddVertex(Point,Normal,Texel) methods.
  //! Warning:
  //! the user is responsible about the orientation of the quadrangle
  //! depending of the order of the created vertex or edges and this
  //! orientation must be coherent with the vertex normal optionnaly
  //! given at each vertex (See the Orientate() methods).
  Standard_EXPORT Graphic3d_ArrayOfQuadrangles(const Standard_Integer maxVertexs, const Standard_Integer maxEdges = 0, const Standard_Boolean hasVNormals = Standard_False, const Standard_Boolean hasVColors = Standard_False, const Standard_Boolean hasTexels = Standard_False);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfQuadrangles,Graphic3d_ArrayOfPrimitives)

protected:




private:




};







#endif // _Graphic3d_ArrayOfQuadrangles_HeaderFile
