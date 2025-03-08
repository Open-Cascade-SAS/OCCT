// Created on: 1993-07-06
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepBuilderAPI_HeaderFile
#define _BRepBuilderAPI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Geom_Plane;

//! The  BRepBuilderAPI  package   provides  an   Application
//! Programming Interface  for the BRep  topology data
//! structure.
//!
//! The API is a set of classes aiming to provide :
//!
//! * High level and simple calls  for the most common
//! operations.
//!
//! *    Keeping   an   access  on    the    low-level
//! implementation of high-level calls.
//!
//! * Examples  of programming of high-level operations
//! from low-level operations.
//!
//! * A complete coverage of modelling :
//!
//! - Creating vertices ,edges, faces, solids.
//!
//! - Sweeping operations.
//!
//! - Boolean operations.
//!
//! - Global properties computation.
//!
//! The API provides  classes to  build  objects:
//!
//! * The  constructors  of the classes  provides  the
//! different constructions methods.
//!
//! * The  class keeps as fields the   different tools
//! used to build the object.
//!
//! *   The class  provides  a  casting  method to get
//! automatically the  result  with  a   function-like
//! call.
//!
//! For example to make a  vertex <V> from a point <P>
//! one can writes :
//!
//! V = BRepBuilderAPI_MakeVertex(P);
//!
//! or
//!
//! BRepBuilderAPI_MakeVertex MV(P);
//! V = MV.Vertex();
//!
//! For tolerances  a default precision is  used which
//! can    be   changed    by    the   packahe  method
//! BRepBuilderAPI::Precision.
//!
//! For error handling the BRepBuilderAPI commands raise only
//! the NotDone error. When Done is false on a command
//! the error description can be asked to the command.
//!
//! In theory the commands can be called with any
//! arguments, argument checking is performed by the
//! command.
class BRepBuilderAPI
{
public:
  DEFINE_STANDARD_ALLOC

  //! Sets    the current plane.
  Standard_EXPORT static void Plane(const Handle(Geom_Plane)& P);

  //! Returns the current plane.
  Standard_EXPORT static const Handle(Geom_Plane)& Plane();

  //! Sets the default precision.  The current Precision
  //! is returned.
  Standard_EXPORT static void Precision(const Standard_Real P);

  //! Returns the default precision.
  Standard_EXPORT static Standard_Real Precision();
};

#endif // _BRepBuilderAPI_HeaderFile
