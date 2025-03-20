// Created on: 1998-06-11
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeBuild_Vertex_HeaderFile
#define _ShapeBuild_Vertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Vertex;
class gp_Pnt;

//! Provides low-level functions used for constructing vertices
class ShapeBuild_Vertex
{
public:
  DEFINE_STANDARD_ALLOC

  //! Combines new vertex from two others. This new one is the
  //! smallest vertex which comprises both of the source vertices.
  //! The function takes into account the positions and tolerances
  //! of the source vertices.
  //! The tolerance of the new vertex will be equal to the minimal
  //! tolerance that is required to comprise source vertices
  //! multiplied by tolFactor (in order to avoid errors because
  //! of discreteness of calculations).
  Standard_EXPORT TopoDS_Vertex CombineVertex(const TopoDS_Vertex& V1,
                                              const TopoDS_Vertex& V2,
                                              const Standard_Real  tolFactor = 1.0001) const;

  //! The same function as above, except that it accepts two points
  //! and two tolerances instead of vertices
  Standard_EXPORT TopoDS_Vertex CombineVertex(const gp_Pnt&       pnt1,
                                              const gp_Pnt&       pnt2,
                                              const Standard_Real tol1,
                                              const Standard_Real tol2,
                                              const Standard_Real tolFactor = 1.0001) const;

protected:
private:
};

#endif // _ShapeBuild_Vertex_HeaderFile
