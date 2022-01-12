// Created on: 1995-03-08
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepPrimAPI_MakeHalfSpace_HeaderFile
#define _BRepPrimAPI_MakeHalfSpace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Solid.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
class TopoDS_Face;
class gp_Pnt;
class TopoDS_Shell;


//! Describes functions to build half-spaces.
//! A half-space is an infinite solid, limited by a surface. It
//! is built from a face or a shell, which bounds it, and with
//! a reference point, which specifies the side of the
//! surface where the matter of the half-space is located.
//! A half-space is a tool commonly used in topological
//! operations to cut another shape.
//! A MakeHalfSpace object provides a framework for:
//! -   defining and implementing the construction of a half-space, and
//! -   consulting the result.
class BRepPrimAPI_MakeHalfSpace  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make a HalfSpace defined with a Face and a Point.
  Standard_EXPORT BRepPrimAPI_MakeHalfSpace(const TopoDS_Face& Face, const gp_Pnt& RefPnt);
  
  //! Make a HalfSpace defined with a Shell and a Point.
  Standard_EXPORT BRepPrimAPI_MakeHalfSpace(const TopoDS_Shell& Shell, const gp_Pnt& RefPnt);
  
  //! Returns the constructed half-space as a solid.
  Standard_EXPORT const TopoDS_Solid& Solid() const;
Standard_EXPORT operator TopoDS_Solid() const;




protected:





private:



  TopoDS_Solid mySolid;


};







#endif // _BRepPrimAPI_MakeHalfSpace_HeaderFile
