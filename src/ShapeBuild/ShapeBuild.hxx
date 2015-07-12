// Created on: 1998-06-03
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

#ifndef _ShapeBuild_HeaderFile
#define _ShapeBuild_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom_Plane;
class ShapeBuild_Vertex;
class ShapeBuild_Edge;
class ShapeBuild_ReShape;


//! This package provides basic building tools for other packages in ShapeHealing.
//! These tools are rather internal for ShapeHealing .
class ShapeBuild 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Rebuilds a shape with substitution of some components
  //! Returns a Geom_Surface which is the Plane XOY (Z positive)
  //! This allows to consider an UV space homologous to a 3D space,
  //! with this support surface
  Standard_EXPORT static Handle(Geom_Plane) PlaneXOY();




protected:





private:




friend class ShapeBuild_Vertex;
friend class ShapeBuild_Edge;
friend class ShapeBuild_ReShape;

};







#endif // _ShapeBuild_HeaderFile
