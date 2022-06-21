// Created on: 1998-07-21
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

#ifndef _ShapeExtend_HeaderFile
#define _ShapeExtend_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ShapeExtend_Status.hxx>
#include <Standard_Boolean.hxx>


//! This package provides general tools and data structures common
//! for other packages in SHAPEWORKS and extending CAS.CADE
//! structures.
//! The following items are provided by this package:
//! - enumeration Status used for coding status flags in methods
//! inside the SHAPEWORKS
//! - enumeration Parametrisation used for setting global parametrisation
//! on the composite surface
//! - class CompositeSurface representing a composite surface
//! made of a grid of surface patches
//! - class WireData representing a wire in the form of ordered
//! list of edges
//! - class MsgRegistrator for attaching messages to the objects
//! - tools for exploring the shapes
//! -       tools for creating       new shapes.
class ShapeExtend 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Inits using of ShapeExtend.
  //! Currently, loads messages output by ShapeHealing algorithms.
  Standard_EXPORT static void Init();
  
  //! Encodes status (enumeration) to a bit flag
  Standard_EXPORT static Standard_Integer EncodeStatus (const ShapeExtend_Status status);
  
  //! Tells if a bit flag contains bit corresponding to enumerated status
  Standard_EXPORT static Standard_Boolean DecodeStatus (const Standard_Integer flag, const ShapeExtend_Status status);

};

#endif // _ShapeExtend_HeaderFile
