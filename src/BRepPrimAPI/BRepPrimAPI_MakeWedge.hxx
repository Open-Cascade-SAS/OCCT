// Created on: 1993-07-22
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

#ifndef _BRepPrimAPI_MakeWedge_HeaderFile
#define _BRepPrimAPI_MakeWedge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Wedge.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Real.hxx>
class Standard_DomainError;
class StdFail_NotDone;
class gp_Ax2;
class BRepPrim_Wedge;
class TopoDS_Shell;
class TopoDS_Solid;


//! Describes functions to build wedges, i.e. boxes with inclined faces.
//! A MakeWedge object provides a framework for:
//! -   defining the construction of a wedge,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeWedge  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make a STEP right angular wedge. (ltx >= 0)
  Standard_EXPORT BRepPrimAPI_MakeWedge(const Standard_Real dx, const Standard_Real dy, const Standard_Real dz, const Standard_Real ltx);
  
  //! Make a STEP right angular wedge. (ltx >= 0)
  Standard_EXPORT BRepPrimAPI_MakeWedge(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz, const Standard_Real ltx);
  
  //! Make a wedge. The face at dy is xmin,zmin xmax,zmax
  Standard_EXPORT BRepPrimAPI_MakeWedge(const Standard_Real dx, const Standard_Real dy, const Standard_Real dz, const Standard_Real xmin, const Standard_Real zmin, const Standard_Real xmax, const Standard_Real zmax);
  
  //! Make a wedge. The face at dy is xmin,zmin xmax,zmax
  Standard_EXPORT BRepPrimAPI_MakeWedge(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz, const Standard_Real xmin, const Standard_Real zmin, const Standard_Real xmax, const Standard_Real zmax);
  
  //! Returns the internal algorithm.
  Standard_EXPORT BRepPrim_Wedge& Wedge();
  
  //! Stores the solid in myShape.
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Returns the constructed box in the form of a shell.
  Standard_EXPORT const TopoDS_Shell& Shell();
Standard_EXPORT operator TopoDS_Shell();
  
  //! Returns the constructed box in the form of a solid.
  Standard_EXPORT const TopoDS_Solid& Solid();
Standard_EXPORT operator TopoDS_Solid();




protected:





private:



  BRepPrim_Wedge myWedge;


};







#endif // _BRepPrimAPI_MakeWedge_HeaderFile
