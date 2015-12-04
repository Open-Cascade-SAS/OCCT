// Created on: 1993-03-24
// Created by: JCV
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

#ifndef _Geom2d_AxisPlacement_HeaderFile
#define _Geom2d_AxisPlacement_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax2d.hxx>
#include <Geom2d_Geometry.hxx>
#include <Standard_Real.hxx>
class gp_Ax2d;
class gp_Pnt2d;
class gp_Dir2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_AxisPlacement;
DEFINE_STANDARD_HANDLE(Geom2d_AxisPlacement, Geom2d_Geometry)

//! Describes an axis in 2D space.
//! An axis is defined by:
//! - its origin, also termed the "Location point" of the axis,
//! - its unit vector, termed the "Direction" of the axis.
//! Note: Geom2d_AxisPlacement axes provide the
//! same kind of "geometric" services as gp_Ax2d axes
//! but have more complex data structures. The
//! geometric objects provided by the Geom2d package
//! use gp_Ax2d objects to include axes in their data
//! structures, or to define an axis of symmetry or axis of rotation.
//! Geom2d_AxisPlacement axes are used in a context
//! where they can be shared by several objects
//! contained inside a common data structure.
class Geom2d_AxisPlacement : public Geom2d_Geometry
{

public:

  
  //! Constructs an axis by conversion of the gp_Ax2d axis A.
  Standard_EXPORT Geom2d_AxisPlacement(const gp_Ax2d& A);
  
  //! Constructs an axis from a given origin P and unit vector V.
  Standard_EXPORT Geom2d_AxisPlacement(const gp_Pnt2d& P, const gp_Dir2d& V);
  
  Standard_EXPORT void Reverse();
  
  //! Reverses the unit vector of this axis.
  //! Note:
  //! - Reverse assigns the result to this axis, while
  //! - Reversed creates a new one.
  Standard_EXPORT Handle(Geom2d_AxisPlacement) Reversed() const;
  
  //! Changes the complete definition of the axis placement.
  Standard_EXPORT void SetAxis (const gp_Ax2d& A);
  

  //! Changes the "Direction" of the axis placement.
  Standard_EXPORT void SetDirection (const gp_Dir2d& V);
  

  //! Changes the "Location" point (origin) of the axis placement.
  Standard_EXPORT void SetLocation (const gp_Pnt2d& P);
  

  //! Computes the angle between the "Direction" of
  //! two axis placement in radians.
  //! The result is comprised between -Pi and Pi.
  Standard_EXPORT Standard_Real Angle (const Handle(Geom2d_AxisPlacement)& Other) const;
  
  //! Converts this axis into a gp_Ax2d axis.
  Standard_EXPORT gp_Ax2d Ax2d() const;
  
  //! Returns the "Direction" of <me>.
  //! -C++: return const&
  Standard_EXPORT gp_Dir2d Direction() const;
  

  //! Returns the "Location" point (origin) of the axis placement.
  //! -C++: return const&
  Standard_EXPORT gp_Pnt2d Location() const;
  
  //! Applies the transformation T to this axis.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this axis.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_AxisPlacement,Geom2d_Geometry)

protected:




private:


  gp_Ax2d axis;


};







#endif // _Geom2d_AxisPlacement_HeaderFile
