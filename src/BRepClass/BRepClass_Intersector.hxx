// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepClass_Intersector_HeaderFile
#define _BRepClass_Intersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Geom2dInt_IntConicCurveOfGInter.hxx>
class gp_Lin2d;
class BRepClass_Edge;
class gp_Dir2d;

//! Intersect an Edge  with a segment.
//! Implement the Intersector2d required by the classifier.
class BRepClass_Intersector : public Geom2dInt_IntConicCurveOfGInter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepClass_Intersector();

  //! Intersect the line segment and the edge.
  Standard_EXPORT void Perform(const gp_Lin2d&       L,
                               const Standard_Real   P,
                               const Standard_Real   Tol,
                               const BRepClass_Edge& E);

  //! Returns in <T>,  <N> and <C>  the tangent,  normal
  //! and  curvature of the edge  <E> at parameter value
  //! <U>.
  Standard_EXPORT void LocalGeometry(const BRepClass_Edge& E,
                                     const Standard_Real   U,
                                     gp_Dir2d&             T,
                                     gp_Dir2d&             N,
                                     Standard_Real&        C) const;

protected:
private:
};

#endif // _BRepClass_Intersector_HeaderFile
