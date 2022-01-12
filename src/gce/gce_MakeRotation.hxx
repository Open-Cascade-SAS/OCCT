// Created on: 1992-08-26
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

#ifndef _gce_MakeRotation_HeaderFile
#define _gce_MakeRotation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf.hxx>
class gp_Lin;
class gp_Ax1;
class gp_Pnt;
class gp_Dir;


//! This class implements elementary construction algorithms for a
//! rotation in 3D space. The result is a gp_Trsf transformation.
//! A MakeRotation object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class gce_MakeRotation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a rotation through angle Angle about the axis defined by the line Line.
  Standard_EXPORT gce_MakeRotation(const gp_Lin& Line, const Standard_Real Angle);
  
  //! Constructs a rotation through angle Angle about the axis defined by the axis Axis.
  Standard_EXPORT gce_MakeRotation(const gp_Ax1& Axis, const Standard_Real Angle);
  

  //! Constructs a rotation through angle Angle about the axis defined by:
  //! the point Point and the unit vector Direc.
  Standard_EXPORT gce_MakeRotation(const gp_Pnt& Point, const gp_Dir& Direc, const Standard_Real Angle);
  
  //! Returns the constructed transformation.
  Standard_EXPORT const gp_Trsf& Value() const;
  
  Standard_EXPORT const gp_Trsf& Operator() const;
Standard_EXPORT operator gp_Trsf() const;




protected:





private:



  gp_Trsf TheRotation;


};







#endif // _gce_MakeRotation_HeaderFile
