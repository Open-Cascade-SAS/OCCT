// Created on: 1991-05-23
// Created by: Didier PIFFAULT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Intf_HeaderFile
#define _Intf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class gp_Pnt;
class gp_XYZ;


//! Interference computation  between polygons, lines  and
//! polyhedra with only  triangular  facets. These objects
//! are polygonal  representations of complex   curves and
//! triangulated representations of complex surfaces.
class Intf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes   the  interference between   two polygons in  2d.
  //! Result : points of intersections and zones of tangence.
  //! Computes the interference  between a polygon or  a straight
  //! line and a polyhedron.   Points of intersection  and zones
  //! of tangence.
  //! Give the plane equation of the triangle <P1> <P2> <P3>.
  Standard_EXPORT static void PlaneEquation (const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3, gp_XYZ& NormalVector, Standard_Real& PolarDistance);
  
  //! Compute if the triangle <P1> <P2> <P3> contain <ThePnt>.
  Standard_EXPORT static Standard_Boolean Contain (const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3, const gp_Pnt& ThePnt);

};

#endif // _Intf_HeaderFile
