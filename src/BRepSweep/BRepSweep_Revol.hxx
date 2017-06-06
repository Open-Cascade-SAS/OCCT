// Created on: 1993-06-22
// Created by: Laurent BOURESCHE
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

#ifndef _BRepSweep_Revol_HeaderFile
#define _BRepSweep_Revol_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepSweep_Rotation.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;
class TopoDS_Shape;
class gp_Ax1;
class Sweep_NumShape;
class TopLoc_Location;


//! Provides natural constructors to build BRepSweep
//! rotated swept Primitives.
class BRepSweep_Revol 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Builds the Revol of meridian S axis A  and angle D. If
  //! C is true S is copied.
  Standard_EXPORT BRepSweep_Revol(const TopoDS_Shape& S, const gp_Ax1& A, const Standard_Real D, const Standard_Boolean C = Standard_False);
  
  //! Builds the Revol of meridian S  axis A and angle 2*Pi.
  //! If C is true S is copied.
  Standard_EXPORT BRepSweep_Revol(const TopoDS_Shape& S, const gp_Ax1& A, const Standard_Boolean C = Standard_False);
  
  //! Returns the TopoDS Shape attached to the Revol.
  Standard_EXPORT TopoDS_Shape Shape();
  
  //! Returns    the  TopoDS  Shape   generated  with  aGenS
  //! (subShape  of the generating shape).
  Standard_EXPORT TopoDS_Shape Shape (const TopoDS_Shape& aGenS);
  
  //! Returns the first shape of the revol  (coinciding with
  //! the generating shape).
  Standard_EXPORT TopoDS_Shape FirstShape();
  
  //! Returns the first shape of the revol  (coinciding with
  //! the generating shape).
  Standard_EXPORT TopoDS_Shape FirstShape (const TopoDS_Shape& aGenS);
  
  //! Returns the TopoDS Shape of the top of the prism.
  Standard_EXPORT TopoDS_Shape LastShape();
  
  //! Returns the  TopoDS  Shape of the top  of  the  prism.
  //! generated  with  aGenS  (subShape  of  the  generating
  //! shape).
  Standard_EXPORT TopoDS_Shape LastShape (const TopoDS_Shape& aGenS);
  
  //! returns the axis
  Standard_EXPORT gp_Ax1 Axe() const;
  
  //! returns the angle.
  Standard_EXPORT Standard_Real Angle() const;

private:

  //! builds the NumShape.
  Standard_EXPORT Sweep_NumShape NumShape (const Standard_Real D) const;
  
  //! Builds the Location
  Standard_EXPORT TopLoc_Location Location (const gp_Ax1& Ax, const Standard_Real D) const;
  
  //! Builds the axis
  Standard_EXPORT gp_Ax1 Axe (const gp_Ax1& Ax, const Standard_Real D) const;
  
  //! computes the angle.
  Standard_EXPORT Standard_Real Angle (const Standard_Real D) const;


  BRepSweep_Rotation myRotation;


};

#endif // _BRepSweep_Revol_HeaderFile
