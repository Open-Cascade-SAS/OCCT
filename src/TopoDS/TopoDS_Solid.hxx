// Created on: 1990-12-17
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

#ifndef _TopoDS_Solid_HeaderFile
#define _TopoDS_Solid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>


//! Describes a solid shape which
//! - references an underlying solid shape with the
//! potential to be given a location and an orientation
//! - has a location for the underlying shape, giving its
//! placement in the local coordinate system
//! - has an orientation for the underlying shape, in
//! terms of its geometry (as opposed to orientation in
//! relation to other shapes).
class TopoDS_Solid  : public TopoDS_Shape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an Undefined Solid.
    TopoDS_Solid();




protected:





private:





};


#include <TopoDS_Solid.lxx>





#endif // _TopoDS_Solid_HeaderFile
