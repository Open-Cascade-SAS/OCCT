// Created on: 1992-09-28
// Created by: Remi GILET
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

#ifndef _GC_MakeRotation_HeaderFile
#define _GC_MakeRotation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Geom_Transformation;
class gp_Lin;
class gp_Ax1;
class gp_Pnt;
class gp_Dir;


//! This class implements elementary construction algorithms for a
//! rotation in 3D space. The result is a
//! Geom_Transformation transformation.
//! A MakeRotation object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class GC_MakeRotation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a rotation through angle Angle about the axis defined by the line Line.
  Standard_EXPORT GC_MakeRotation(const gp_Lin& Line, const Standard_Real Angle);
  
  //! Constructs a rotation through angle Angle about the axis defined by the axis Axis.
  Standard_EXPORT GC_MakeRotation(const gp_Ax1& Axis, const Standard_Real Angle);
  
  //! Constructs a rotation through angle Angle about the axis
  //! defined by the point Point and the unit vector Direc.
  Standard_EXPORT GC_MakeRotation(const gp_Pnt& Point, const gp_Dir& Direc, const Standard_Real Angle);
  
  //! Returns the constructed transformation.
  Standard_EXPORT const Handle(Geom_Transformation)& Value() const;
operator const Handle(Geom_Transformation)& () const { return Value(); }




protected:





private:



  Handle(Geom_Transformation) TheRotation;


};







#endif // _GC_MakeRotation_HeaderFile
