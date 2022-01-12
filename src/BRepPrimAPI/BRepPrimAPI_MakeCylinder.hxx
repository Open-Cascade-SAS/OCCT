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

#ifndef _BRepPrimAPI_MakeCylinder_HeaderFile
#define _BRepPrimAPI_MakeCylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Cylinder.hxx>
#include <BRepPrimAPI_MakeOneAxis.hxx>
class gp_Ax2;


//! Describes functions to build cylinders or portions of  cylinders.
//! A MakeCylinder object provides a framework for:
//! -   defining the construction of a cylinder,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeCylinder  : public BRepPrimAPI_MakeOneAxis
{
public:

  DEFINE_STANDARD_ALLOC

  //! Make a cylinder.
  //! @param R [in] cylinder radius
  //! @param H [in] cylinder height
  Standard_EXPORT BRepPrimAPI_MakeCylinder(const Standard_Real R, const Standard_Real H);

  //! Make a cylinder (part cylinder).
  //! @param R     [in] cylinder radius
  //! @param H     [in] cylinder height
  //! @param Angle [in] defines the missing portion of the cylinder
  Standard_EXPORT BRepPrimAPI_MakeCylinder(const Standard_Real R, const Standard_Real H, const Standard_Real Angle);

  //! Make a cylinder of radius R and length H.
  //! @param Axes [in] coordinate system for the construction of the cylinder
  //! @param R    [in] cylinder radius
  //! @param H    [in] cylinder height
  Standard_EXPORT BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes, const Standard_Real R, const Standard_Real H);

  //! Make a cylinder   of  radius R  and  length H with
  //! angle  H.
  //! Constructs
  //! -   a cylinder of radius R and height H, or
  //! -   a portion of cylinder of radius R and height H, and of
  //! the angle Angle defining the missing portion of the cylinder.
  //! The cylinder is constructed about the "Z Axis" of either:
  //! -   the global coordinate system, or
  //! -   the local coordinate system Axes.
  //! It is limited in this coordinate system as follows:
  //! -   in the v parametric direction (the Z axis), by the two
  //! parameter values 0 and H,
  //! -   and in the u parametric direction (the rotation angle
  //! around the Z Axis), in the case of a portion of a
  //! cylinder, by the two parameter values 0 and Angle.
  //! Angle is given in radians.
  //! The resulting shape is composed of:
  //! -   a lateral cylindrical face,
  //! -   two planar faces in the planes z = 0 and z = H
  //! (in the case of a complete cylinder, these faces are circles), and
  //! -   in case of a portion of a cylinder, two additional
  //! planar faces to close the shape.(two rectangles in the
  //! planes u = 0 and u = Angle).
  //! Exceptions Standard_DomainError if:
  //! -   R is less than or equal to Precision::Confusion(), or
  //! -   H is less than or equal to Precision::Confusion().
  Standard_EXPORT BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes, const Standard_Real R, const Standard_Real H, const Standard_Real Angle);
  
  //! Returns the algorithm.
  Standard_EXPORT Standard_Address OneAxis();
  
  //! Returns the algorithm.
  Standard_EXPORT BRepPrim_Cylinder& Cylinder();




protected:





private:



  BRepPrim_Cylinder myCylinder;


};







#endif // _BRepPrimAPI_MakeCylinder_HeaderFile
