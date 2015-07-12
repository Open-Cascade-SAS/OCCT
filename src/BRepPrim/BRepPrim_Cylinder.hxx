// Created on: 1992-11-05
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepPrim_Cylinder_HeaderFile
#define _BRepPrim_Cylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <BRepPrim_Revolution.hxx>
class Standard_DomainError;
class gp_Ax2;
class gp_Pnt;
class TopoDS_Face;


//! Cylinder primitive.
class BRepPrim_Cylinder  : public BRepPrim_Revolution
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! the STEP definition
  //! Position : center of a Face and Axis
  //! Radius : radius of cylinder
  //! Height : distance between faces
  //! on positive side
  //!
  //! Errors : Height < Resolution
  //! Radius < Resolution
  Standard_EXPORT BRepPrim_Cylinder(const gp_Ax2& Position, const Standard_Real Radius, const Standard_Real Height);
  
  //! infinite Cylinder at origin on Z negative
  Standard_EXPORT BRepPrim_Cylinder(const Standard_Real Radius);
  
  //! infinite Cylinder at Center on Z negative
  Standard_EXPORT BRepPrim_Cylinder(const gp_Pnt& Center, const Standard_Real Radius);
  
  //! infinite Cylinder at Axes on Z negative
  Standard_EXPORT BRepPrim_Cylinder(const gp_Ax2& Axes, const Standard_Real Radius);
  
  //! create a Cylinder  at origin on Z  axis, of
  //! height H and radius R
  //! Error  : Radius  < Resolution
  //! H < Resolution
  //! H negative
  Standard_EXPORT BRepPrim_Cylinder(const Standard_Real R, const Standard_Real H);
  
  //! same as above but at a given point
  Standard_EXPORT BRepPrim_Cylinder(const gp_Pnt& Center, const Standard_Real R, const Standard_Real H);
  
  //! The surface normal should be directed  towards the
  //! outside.
  Standard_EXPORT virtual TopoDS_Face MakeEmptyLateralFace() const Standard_OVERRIDE;




protected:





private:

  
  Standard_EXPORT void SetMeridian();


  Standard_Real myRadius;


};







#endif // _BRepPrim_Cylinder_HeaderFile
