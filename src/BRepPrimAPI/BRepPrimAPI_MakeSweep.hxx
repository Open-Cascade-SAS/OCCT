// Created on: 1994-02-18
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepPrimAPI_MakeSweep_HeaderFile
#define _BRepPrimAPI_MakeSweep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepBuilderAPI_MakeShape.hxx>
class TopoDS_Shape;


//! The abstract class MakeSweep is
//! the root class of swept primitives.
//! Sweeps are objects you obtain by sweeping a profile along a path.
//! The profile can be any topology and the path is usually a curve or
//! a wire. The profile generates objects according to the following rules:
//! -      Vertices generate Edges
//! -      Edges generate Faces.
//! -      Wires generate Shells.
//! -      Faces generate Solids.
//! -      Shells generate Composite  Solids.
//! You are not allowed to sweep Solids and Composite Solids.
//! Two kinds of sweeps are implemented in the BRepPrimAPI package:
//! -      The linear sweep called a   Prism
//! -      The rotational sweep    called a Revol
//! Swept constructions along complex profiles such as BSpline curves
//! are also available in the BRepOffsetAPI package..
class BRepPrimAPI_MakeSweep  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the  TopoDS  Shape of the bottom of the sweep.
  Standard_EXPORT virtual TopoDS_Shape FirstShape() = 0;
  
  //! Returns the TopoDS Shape of the top of the sweep.
  Standard_EXPORT virtual TopoDS_Shape LastShape() = 0;




protected:





private:





};







#endif // _BRepPrimAPI_MakeSweep_HeaderFile
