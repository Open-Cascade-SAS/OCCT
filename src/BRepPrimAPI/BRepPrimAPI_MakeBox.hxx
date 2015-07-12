// Created on: 1993-07-21
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

#ifndef _BRepPrimAPI_MakeBox_HeaderFile
#define _BRepPrimAPI_MakeBox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Wedge.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Real.hxx>
class Standard_DomainError;
class Standard_OutOfRange;
class StdFail_NotDone;
class gp_Pnt;
class gp_Ax2;
class BRepPrim_Wedge;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_Face;


//! Describes functions to build parallelepiped boxes.
//! A MakeBox object provides a framework for:
//! -   defining the construction of a box,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeBox  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make a box with a corner at 0,0,0 and the other dx,dy,dz
  Standard_EXPORT BRepPrimAPI_MakeBox(const Standard_Real dx, const Standard_Real dy, const Standard_Real dz);
  
  //! Make a box with a corner at P and size dx, dy, dz.
  Standard_EXPORT BRepPrimAPI_MakeBox(const gp_Pnt& P, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz);
  
  //! Make a box with corners P1,P2.
  Standard_EXPORT BRepPrimAPI_MakeBox(const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Ax2 is the left corner and the axis.
  //! Constructs a box such that its sides are parallel to the axes of
  //! -   the global coordinate system, or
  //! -   the local coordinate system Axis. and
  //! -   with a corner at (0, 0, 0) and of size (dx, dy, dz), or
  //! -   with a corner at point P and of size (dx, dy, dz), or
  //! -   with corners at points P1 and P2.
  //! Exceptions
  //! Standard_DomainError if: dx, dy, dz are less than or equal to
  //! Precision::Confusion(), or
  //! -   the vector joining the points P1 and P2 has a
  //! component projected onto the global coordinate
  //! system less than or equal to Precision::Confusion().
  //! In these cases, the box would be flat.
  Standard_EXPORT BRepPrimAPI_MakeBox(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz);
  
  //! Returns the internal algorithm.
  Standard_EXPORT BRepPrim_Wedge& Wedge();
  
  //! Stores the solid in myShape.
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Returns the constructed box as a shell.
  Standard_EXPORT const TopoDS_Shell& Shell();
Standard_EXPORT operator TopoDS_Shell();
  
  //! Returns the constructed box as a solid.
  Standard_EXPORT const TopoDS_Solid& Solid();
Standard_EXPORT operator TopoDS_Solid();
  
  //! Returns ZMin face
  Standard_EXPORT const TopoDS_Face& BottomFace();
  
  //! Returns XMin face
  Standard_EXPORT const TopoDS_Face& BackFace();
  
  //! Returns XMax face
  Standard_EXPORT const TopoDS_Face& FrontFace();
  
  //! Returns YMin face
  Standard_EXPORT const TopoDS_Face& LeftFace();
  
  //! Returns YMax face
  Standard_EXPORT const TopoDS_Face& RightFace();
  
  //! Returns ZMax face
  Standard_EXPORT const TopoDS_Face& TopFace();




protected:





private:



  BRepPrim_Wedge myWedge;


};







#endif // _BRepPrimAPI_MakeBox_HeaderFile
