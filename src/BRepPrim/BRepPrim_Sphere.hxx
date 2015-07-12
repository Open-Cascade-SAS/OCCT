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

#ifndef _BRepPrim_Sphere_HeaderFile
#define _BRepPrim_Sphere_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <BRepPrim_Revolution.hxx>
class Standard_DomainError;
class gp_Pnt;
class gp_Ax2;
class TopoDS_Face;


//! Implements the sphere primitive
class BRepPrim_Sphere  : public BRepPrim_Revolution
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Sphere at  origin with  Radius. The axes
  //! of the sphere are the  reference axes. An error is
  //! raised if the radius is < Resolution.
  Standard_EXPORT BRepPrim_Sphere(const Standard_Real Radius);
  
  //! Creates a Sphere with Center and Radius.  Axes are
  //! the   referrence    axes.   This    is the    STEP
  //! constructor.
  Standard_EXPORT BRepPrim_Sphere(const gp_Pnt& Center, const Standard_Real Radius);
  
  //! Creates a sphere with given axes system.
  Standard_EXPORT BRepPrim_Sphere(const gp_Ax2& Axes, const Standard_Real Radius);
  
  //! The surface normal should be directed  towards the
  //! outside.
  Standard_EXPORT virtual TopoDS_Face MakeEmptyLateralFace() const Standard_OVERRIDE;




protected:





private:

  
  Standard_EXPORT void SetMeridian();


  Standard_Real myRadius;


};







#endif // _BRepPrim_Sphere_HeaderFile
