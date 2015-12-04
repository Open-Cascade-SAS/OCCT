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

#ifndef _Graphic3d_ArrayOfTriangleFans_HeaderFile
#define _Graphic3d_ArrayOfTriangleFans_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


class Graphic3d_ArrayOfTriangleFans;
DEFINE_STANDARD_HANDLE(Graphic3d_ArrayOfTriangleFans, Graphic3d_ArrayOfPrimitives)

//! Contains triangles fan array definition
class Graphic3d_ArrayOfTriangleFans : public Graphic3d_ArrayOfPrimitives
{

public:

  
  //! Creates an array of triangle fans,
  //! a polygon can be filled as:
  //! 1) creating a single fan defined with his vertexs.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfTriangleFans(7)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x7,y7,z7)
  //! 2) creating separate fans defined with a predefined
  //! number of fans and the number of vertex per fan.
  //! i.e:
  //! myArray = Graphic3d_ArrayOfTriangleFans(8,2)
  //! myArray->AddBound(4)
  //! myArray->AddVertex(x1,y1,z1)
  //! ....
  //! myArray->AddVertex(x4,y4,z4)
  //! myArray->AddBound(4)
  //! myArray->AddVertex(x5,y5,z5)
  //! ....
  //! myArray->AddVertex(x8,y8,z8)
  //!
  //! <maxVertexs> defined the maximun allowed vertex number in the array.
  //! <maxFans> defined the maximun allowed fan number in the array.
  //! The number of triangle really drawn is :
  //! VertexNumber()-2*Min(1,BoundNumber())
  Standard_EXPORT Graphic3d_ArrayOfTriangleFans(const Standard_Integer maxVertexs, const Standard_Integer maxFans = 0, const Standard_Boolean hasVNormals = Standard_False, const Standard_Boolean hasVColors = Standard_False, const Standard_Boolean hasFColors = Standard_False, const Standard_Boolean hasTexels = Standard_False);




  DEFINE_STANDARD_RTTIEXT(Graphic3d_ArrayOfTriangleFans,Graphic3d_ArrayOfPrimitives)

protected:




private:




};







#endif // _Graphic3d_ArrayOfTriangleFans_HeaderFile
