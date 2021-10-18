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

#ifndef _Geom_Axis2Placement_HeaderFile
#define _Geom_Axis2Placement_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Dir.hxx>
#include <Geom_AxisPlacement.hxx>
class gp_Ax2;
class gp_Pnt;
class gp_Trsf;
class Geom_Geometry;


class Geom_Axis2Placement;
DEFINE_STANDARD_HANDLE(Geom_Axis2Placement, Geom_AxisPlacement)

//! Describes a right-handed coordinate system in 3D space.
//! A coordinate system is defined by:
//! - its origin, also termed the "Location point" of the coordinate system,
//! - three orthogonal unit vectors, termed respectively
//! the "X Direction", "Y Direction" and "Direction" (or
//! "main Direction") of the coordinate system.
//! As a Geom_Axis2Placement coordinate system is
//! right-handed, its "Direction" is always equal to the
//! cross product of its "X Direction" and "Y Direction".
//! The "Direction" of a coordinate system is called the
//! "main Direction" because when this unit vector is
//! modified, the "X Direction" and "Y Direction" are
//! recomputed, whereas when the "X Direction" or "Y
//! Direction" is changed, the "main Direction" is
//! retained. The "main Direction" is also the "Z Direction".
//! Note: Geom_Axis2Placement coordinate systems
//! provide the same kind of "geometric" services as
//! gp_Ax2 coordinate systems but have more complex
//! data structures. The geometric objects provided by
//! the Geom package use gp_Ax2 objects to include
//! coordinate systems in their data structures, or to
//! define the geometric transformations, which are applied to them.
//! Geom_Axis2Placement coordinate systems are
//! used in a context where they can be shared by
//! several objects contained inside a common data structure.
class Geom_Axis2Placement : public Geom_AxisPlacement
{

public:

  
  //! Returns a transient copy of A2.
  Standard_EXPORT Geom_Axis2Placement(const gp_Ax2& A2);
  

  //! P is the origin of the axis placement, N is the main
  //! direction of the axis placement and Vx is the "XDirection".
  //! If the two directions N and Vx are not orthogonal the
  //! "XDirection" is computed as follow :
  //! XDirection = N ^ (Vx ^ N).
  //! Raised if N and Vx are parallel.
  Standard_EXPORT Geom_Axis2Placement(const gp_Pnt& P, const gp_Dir& N, const gp_Dir& Vx);
  
  //! Assigns the origin and the three unit vectors of A2 to
  //! this coordinate system.
  Standard_EXPORT void SetAx2 (const gp_Ax2& A2);
  

  //! Changes the main direction of the axis placement.
  //! The "Xdirection" is modified :
  //! New XDirection = V ^ (Previous_Xdirection ^ V).
  //!
  //! Raised if V and the previous "XDirection" are parallel
  //! because it is impossible to calculate the new "XDirection"
  //! and the new "YDirection".
  Standard_EXPORT void SetDirection (const gp_Dir& V) Standard_OVERRIDE;
  

  //! Changes the "XDirection" of the axis placement, Vx is the
  //! new "XDirection". If Vx is not normal to the main direction
  //! then "XDirection" is computed as follow :
  //! XDirection = Direction ^ ( Vx ^ Direction).
  //! The main direction is not modified.
  //! Raised if Vx and "Direction"  are parallel.
  Standard_EXPORT void SetXDirection (const gp_Dir& Vx);
  

  //! Changes the "YDirection" of the axis placement, Vy is the
  //! new "YDirection". If Vy is not normal to the main direction
  //! then "YDirection" is computed as follow :
  //! YDirection = Direction ^ ( Vy ^ Direction).
  //! The main direction is not modified. The "XDirection" is
  //! modified.
  //! Raised if Vy and the main direction are parallel.
  Standard_EXPORT void SetYDirection (const gp_Dir& Vy);
  
  //! Returns a non transient copy of <me>.
  Standard_EXPORT gp_Ax2 Ax2() const;
  
  //! Returns the "XDirection". This is a unit vector.
  Standard_EXPORT const gp_Dir& XDirection() const;
  
  //! Returns the "YDirection". This is a unit vector.
  Standard_EXPORT const gp_Dir& YDirection() const;
  

  //! Transforms an axis placement with a Trsf.
  //! The "Location" point, the "XDirection" and the
  //! "YDirection" are transformed with T.  The resulting
  //! main "Direction" of <me> is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this coordinate system.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_Axis2Placement,Geom_AxisPlacement)

protected:




private:

  
  Standard_EXPORT Geom_Axis2Placement(const gp_Pnt& P, const gp_Dir& Vz, const gp_Dir& Vx, const gp_Dir& Vy);

  gp_Dir vxdir;
  gp_Dir vydir;


};







#endif // _Geom_Axis2Placement_HeaderFile
