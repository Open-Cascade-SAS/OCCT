// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_Ax2d_HeaderFile
#define _gp_Ax2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class gp_Pnt2d;
class gp_Dir2d;
class gp_Trsf2d;
class gp_Vec2d;



//! Describes an axis in the plane (2D space).
//! An axis is defined by:
//! -   its origin (also referred to as its "Location point"),   and
//! -   its unit vector (referred to as its "Direction").
//! An axis implicitly defines a direct, right-handed
//! coordinate system in 2D space by:
//! -   its origin,
//! - its "Direction" (giving the "X Direction" of the coordinate system), and
//! -   the unit vector normal to "Direction" (positive angle
//! measured in the trigonometric sense).
//! An axis is used:
//! -   to describe 2D geometric entities (for example, the
//! axis which defines angular coordinates on a circle).
//! It serves for the same purpose as the STEP function
//! "axis placement one axis", or
//! -   to define geometric transformations (axis of
//! symmetry, axis of rotation, and so on).
//! Note: to define a left-handed 2D coordinate system, use gp_Ax22d.
class gp_Ax2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an axis object representing X axis of
  //! the reference co-ordinate system.
    gp_Ax2d();
  

  //! Creates an Ax2d. <P> is the "Location" point of
  //! the axis placement and V is the "Direction" of
  //! the axis placement.
    gp_Ax2d(const gp_Pnt2d& P, const gp_Dir2d& V);
  
  //! Changes the "Location" point (origin) of <me>.
    void SetLocation (const gp_Pnt2d& Locat);
  
  //! Changes the direction of <me>.
    void SetDirection (const gp_Dir2d& V);
  
  //! Returns the origin of <me>.
    const gp_Pnt2d& Location() const;
  
  //! Returns the direction of <me>.
    const gp_Dir2d& Direction() const;
  

  //! Returns True if  :
  //! . the angle between <me> and <Other> is lower or equal
  //! to <AngularTolerance> and
  //! . the distance between <me>.Location() and <Other> is lower
  //! or equal to <LinearTolerance> and
  //! . the distance between <Other>.Location() and <me> is lower
  //! or equal to LinearTolerance.
  Standard_EXPORT Standard_Boolean IsCoaxial (const gp_Ax2d& Other, const Standard_Real AngularTolerance, const Standard_Real LinearTolerance) const;
  
  //! Returns true if this axis and the axis Other are normal to
  //! each other. That is, if the angle between the two axes is equal to Pi/2 or -Pi/2.
  //! Note: the tolerance criterion is given by AngularTolerance.
    Standard_Boolean IsNormal (const gp_Ax2d& Other, const Standard_Real AngularTolerance) const;
  
  //! Returns true if this axis and the axis Other are parallel,
  //! and have opposite orientations. That is, if the angle
  //! between the two axes is equal to Pi or -Pi.
  //! Note: the tolerance criterion is given by AngularTolerance.
    Standard_Boolean IsOpposite (const gp_Ax2d& Other, const Standard_Real AngularTolerance) const;
  
  //! Returns true if this axis and the axis Other are parallel,
  //! and have either the same or opposite orientations. That
  //! is, if the angle between the two axes is equal to 0, Pi or -Pi.
  //! Note: the tolerance criterion is given by AngularTolerance.
    Standard_Boolean IsParallel (const gp_Ax2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Computes the angle, in radians, between this axis and
  //! the axis Other. The value of the angle is between -Pi and Pi.
    Standard_Real Angle (const gp_Ax2d& Other) const;
  
  //! Reverses the direction of <me> and assigns the result to this axis.
    void Reverse();
  

  //! Computes a new axis placement with a direction opposite to
  //! the direction of <me>.
    gp_Ax2d Reversed() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt2d& P);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point P which is the
  //! center of the symmetry.
  Standard_EXPORT gp_Ax2d Mirrored (const gp_Pnt2d& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2d& A);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry.
  Standard_EXPORT gp_Ax2d Mirrored (const gp_Ax2d& A) const;
  
    void Rotate (const gp_Pnt2d& P, const Standard_Real Ang);
  

  //! Rotates an axis placement. <P> is the center of the
  //! rotation . Ang is the angular value of the rotation
  //! in radians.
    gp_Ax2d Rotated (const gp_Pnt2d& P, const Standard_Real Ang) const;
  
  Standard_EXPORT void Scale (const gp_Pnt2d& P, const Standard_Real S);
  

  //! Applies a scaling transformation on the axis placement.
  //! The "Location" point of the axisplacement is modified.
  //! The "Direction" is reversed if the scale is negative.
    gp_Ax2d Scaled (const gp_Pnt2d& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf2d& T);
  
  //! Transforms an axis placement with a Trsf.
    gp_Ax2d Transformed (const gp_Trsf2d& T) const;
  
    void Translate (const gp_Vec2d& V);
  

  //! Translates an axis placement in the direction of the vector
  //! <V>. The magnitude of the translation is the vector's magnitude.
    gp_Ax2d Translated (const gp_Vec2d& V) const;
  
    void Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  

  //! Translates an axis placement from the point <P1> to the
  //! point <P2>.
    gp_Ax2d Translated (const gp_Pnt2d& P1, const gp_Pnt2d& P2) const;




protected:





private:



  gp_Pnt2d loc;
  gp_Dir2d vdir;


};


#include <gp_Ax2d.lxx>





#endif // _gp_Ax2d_HeaderFile
