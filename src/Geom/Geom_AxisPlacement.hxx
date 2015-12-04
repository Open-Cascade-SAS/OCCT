// Created on: 1993-03-09
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

#ifndef _Geom_AxisPlacement_HeaderFile
#define _Geom_AxisPlacement_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax1.hxx>
#include <Geom_Geometry.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;
class gp_Ax1;
class gp_Dir;
class gp_Pnt;


class Geom_AxisPlacement;
DEFINE_STANDARD_HANDLE(Geom_AxisPlacement, Geom_Geometry)

//! The abstract class AxisPlacement describes the
//! common behavior of positioning systems in 3D space,
//! such as axis or coordinate systems.
//! The Geom package provides two implementations of
//! 3D positioning systems:
//! - the axis (Geom_Axis1Placement class), which is defined by:
//! - its origin, also termed the "Location point" of the  axis,
//! - its unit vector, termed the "Direction" or "main
//! Direction" of the axis;
//! - the right-handed coordinate system
//! (Geom_Axis2Placement class), which is defined by:
//! - its origin, also termed the "Location point" of the coordinate system,
//! - three orthogonal unit vectors, termed
//! respectively the "X Direction", the "Y Direction"
//! and the "Direction" of the coordinate system. As
//! the coordinate system is right-handed, these
//! unit vectors have the following relation:
//! "Direction" = "X Direction" ^
//! "Y Direction". The "Direction" is also
//! called the "main Direction" because, when the
//! unit vector is modified, the "X Direction" and "Y
//! Direction" are recomputed, whereas when the "X
//! Direction" or "Y Direction" is modified, the "main Direction" does not change.
//! The axis whose origin is the origin of the positioning
//! system and whose unit vector is its "main Direction" is
//! also called the "Axis" or "main Axis" of the positioning system.
class Geom_AxisPlacement : public Geom_Geometry
{

public:

  
  //! Assigns A1 as the "main Axis" of this positioning system. This modifies
  //! - its origin, and
  //! - its "main Direction".
  //! If this positioning system is a
  //! Geom_Axis2Placement, then its "X Direction" and
  //! "Y Direction" are recomputed.
  //! Exceptions
  //! For a Geom_Axis2Placement:
  //! Standard_ConstructionError if A1 and the
  //! previous "X Direction" of the coordinate system are parallel.
  Standard_EXPORT void SetAxis (const gp_Ax1& A1);
  

  //! Changes the direction of the axis placement.
  //! If <me> is an axis placement two axis the main "Direction"
  //! is modified and the "XDirection" and "YDirection" are
  //! recomputed.
  //! Raises ConstructionError only for an axis placement two axis if V and the
  //! previous "XDirection" are parallel because it is not possible
  //! to calculate the new "XDirection" and the new "YDirection".
  Standard_EXPORT virtual void SetDirection (const gp_Dir& V) = 0;
  

  //! Assigns the point P as the origin of this positioning  system.
  Standard_EXPORT void SetLocation (const gp_Pnt& P);
  
  //! Computes the angular value, in radians, between the
  //! "main Direction" of this positioning system and that
  //! of positioning system Other. The result is a value between 0 and Pi.
  Standard_EXPORT Standard_Real Angle (const Handle(Geom_AxisPlacement)& Other) const;
  
  //! Returns the main axis of the axis placement.
  //! For an "Axis2placement" it is the main axis (Location, Direction ).
  //! For an "Axis1Placement" this method returns a copy of <me>.
  Standard_EXPORT const gp_Ax1& Axis() const;
  

  //! Returns the main "Direction" of an axis placement.
  Standard_EXPORT gp_Dir Direction() const;
  

  //! Returns the Location point (origin) of the axis placement.
  Standard_EXPORT gp_Pnt Location() const;




  DEFINE_STANDARD_RTTIEXT(Geom_AxisPlacement,Geom_Geometry)

protected:


  gp_Ax1 axis;


private:




};







#endif // _Geom_AxisPlacement_HeaderFile
