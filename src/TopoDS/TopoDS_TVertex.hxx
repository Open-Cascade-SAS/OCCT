// Created on: 1990-12-13
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TopoDS_TVertex_HeaderFile
#define _TopoDS_TVertex_HeaderFile

#include <Standard.hxx>

#include <TopoDS_TShape.hxx>
#include <TopAbs_ShapeEnum.hxx>


class TopoDS_TVertex;

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

DEFINE_STANDARD_HANDLE(TopoDS_TVertex, TopoDS_TShape)

//! A  Vertex is a topological  point in  two or three
//! dimensions.
class TopoDS_TVertex : public TopoDS_TShape
{

public:

  
  //! Returns VERTEX.
  Standard_EXPORT TopAbs_ShapeEnum ShapeType() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TopoDS_TVertex,TopoDS_TShape)

protected:

  
  //! Construct a vertex.
    TopoDS_TVertex();



private:




};


#include <TopoDS_TVertex.lxx>





#endif // _TopoDS_TVertex_HeaderFile
