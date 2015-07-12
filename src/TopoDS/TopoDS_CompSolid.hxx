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

#ifndef _TopoDS_CompSolid_HeaderFile
#define _TopoDS_CompSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>


//! Describes a composite solid which
//! - references an underlying composite solid with the
//! potential to be given a location and an orientation
//! - has a location for the underlying composite solid,
//! giving its placement in the local coordinate system
//! - has an orientation for the underlying composite
//! solid, in terms of its geometry (as opposed to
//! orientation in relation to other shapes).
//! Casts shape S to the more specialized return type, CompSolid.
class TopoDS_CompSolid  : public TopoDS_Shape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an Undefined CompSolid.
    TopoDS_CompSolid();




protected:





private:





};


#include <TopoDS_CompSolid.lxx>





#endif // _TopoDS_CompSolid_HeaderFile
