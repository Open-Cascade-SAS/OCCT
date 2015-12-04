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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


class Graphic3d_ArrayOfSegments;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfSegments, Graphic3d_ArrayOfPrimitives)

//! Contains segments array definition
class Graphic3d_ArrayOfSegments : public Graphic3d_ArrayOfPrimitives
{

public:

  
  //! Creates an array of segments,
  //! a segment can be filled as:
  //! 1) creating a set of segments defined with his vertexs.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfSegments(4)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x4,y4,z4)
  //! 2) creating a set of indexed segments defined with his vertex
  //! ans edges.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfSegments(4,0,8)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x4,y4,z4)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(3)
  //! myArray->AddEdge(4)
  //! myArray->AddEdge(2)
  //! myArray->AddEdge(4)
  //! myArray->AddEdge(1)
  //! myArray->AddEdge(3)
  //!
  //! <maxVertexs> defined the maximun allowed vertex number in the array.
  //! <maxEdges> defined the maximun allowed edge number in the array.
  //! Warning:
  //! When <hasVColors> is TRUE , you must use only
  //! AddVertex(Point,Color) method
  Standard_EXPORT Graphic3d_ArrayOfSegments(const Standard_Integer maxVertexs, const Standard_Integer maxEdges = 0, const Standard_Boolean hasVColors = Standard_False);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfSegments,Graphic3d_ArrayOfPrimitives)

protected:




private:




};







#endif // _Graphic3d_ArrayOfSegments_HeaderFile
