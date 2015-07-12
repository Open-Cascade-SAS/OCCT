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

#ifndef _TopoDS_Shell_HeaderFile
#define _TopoDS_Shell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>


//! Describes a shell which
//! - references an underlying shell with the potential to
//! be given a location and an orientation
//! - has a location for the underlying shell, giving its
//! placement in the local coordinate system
//! - has an orientation for the underlying shell, in terms
//! of its geometry (as opposed to orientation in relation to other shapes).
class TopoDS_Shell  : public TopoDS_Shape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an Undefined Shell.
    TopoDS_Shell();




protected:





private:





};


#include <TopoDS_Shell.lxx>





#endif // _TopoDS_Shell_HeaderFile
