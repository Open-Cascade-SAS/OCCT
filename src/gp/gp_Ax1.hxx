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

#ifndef _gp_Ax1_HeaderFile
#define _gp_Ax1_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class gp_Pnt;
class gp_Dir;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;


//! Describes an axis in 3D space.
//! An axis is defined by:
//! -   its origin (also referred to as its "Location point"), and
//! -   its unit vector (referred to as its "Direction" or "main   Direction").
//! An axis is used:
//! -   to describe 3D geometric entities (for example, the
//! axis of a revolution entity). It serves the same purpose
//! as the STEP function "axis placement one axis", or
//! -   to define geometric transformations (axis of
//! symmetry, axis of rotation, and so on).
//! For example, this entity can be used to locate a geometric entity
//! or to define a symmetry axis.
class gp_Ax1 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an axis object representing Z axis of
  //! the reference co-ordinate system.
    gp_Ax1();
  

  //! P is the location point and V is the direction of <me>.
    gp_Ax1(const gp_Pnt& P, const gp_Dir& V);
  
  //! Assigns V as the "Direction"  of this axis.
    void SetDirection (const gp_Dir& V);
  
  //! Assigns  P as the origin of this axis.
    void SetLocation (const gp_Pnt& P);
  
  //! Returns the direction of <me>.
    const gp_Dir& Direction() const;
  
  //! Returns the location point of <me>.
    const gp_Pnt& Location() const;
  

  //! Returns True if  :
  //! . the angle between <me> and <Other> is lower or equal
  //! to <AngularTolerance> and
  //! . the distance between <me>.Location() and <Other> is lower
  //! or equal to <LinearTolerance> and
  //! . the distance between <Other>.Location() and <me> is lower
  //! or equal to LinearTolerance.
  Standard_EXPORT Standard_Boolean IsCoaxial (const gp_Ax1& Other, const Standard_Real AngularTolerance, const Standard_Real LinearTolerance) const;
  

  //! Returns True if the direction of the <me> and <Other>
  //! are normal to each other.
  //! That is, if the angle between the two axes is equal to Pi/2.
  //! Note: the tolerance criterion is given by AngularTolerance..
    Standard_Boolean IsNormal (const gp_Ax1& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if the direction of <me> and <Other> are
  //! parallel with opposite orientation. That is, if the angle
  //! between the two axes is equal to Pi.
  //! Note: the tolerance criterion is given by AngularTolerance.
    Standard_Boolean IsOpposite (const gp_Ax1& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if the direction of <me> and <Other> are
  //! parallel with same orientation or opposite orientation. That
  //! is, if the angle between the two axes is equal to 0 or Pi.
  //! Note: the tolerance criterion is given by
  //! AngularTolerance.
    Standard_Boolean IsParallel (const gp_Ax1& Other, const Standard_Real AngularTolerance) const;
  

  //! Computes the angular value, in radians, between <me>.Direction() and
  //! <Other>.Direction(). Returns the angle between 0 and 2*PI
  //! radians.
    Standard_Real Angle (const gp_Ax1& Other) const;
  
  //! Reverses the unit vector of this axis.
  //! and  assigns the result to this axis.
    void Reverse();
  
  //! Reverses the unit vector of this axis and creates a new one.
    gp_Ax1 Reversed() const;
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point P which is the
  //! center of the symmetry and assigns the result to this axis.
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  
  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point P which is the
  //! center of the symmetry and creates a new axis.
  Standard_EXPORT gp_Ax1 Mirrored (const gp_Pnt& P) const;
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry and assigns the result to this axis.
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry and creates a new axis.
  Standard_EXPORT gp_Ax1 Mirrored (const gp_Ax1& A1) const;
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to a plane. The axis placement
  //! <A2> locates the plane of the symmetry :
  //! (Location, XDirection, YDirection) and assigns the result to this axis.
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to a plane. The axis placement
  //! <A2> locates the plane of the symmetry :
  //! (Location, XDirection, YDirection) and creates a new axis.
  Standard_EXPORT gp_Ax1 Mirrored (const gp_Ax2& A2) const;
  
  //! Rotates this axis at an angle Ang (in radians) about the axis A1
  //! and assigns the result to this axis.
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  
  //! Rotates this axis at an angle Ang (in radians) about the axis A1
  //! and creates a new one.
    gp_Ax1 Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  

  //! Applies a scaling transformation to this axis with:
  //! -   scale factor S, and
  //! -   center P and assigns the result to this axis.
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Applies a scaling transformation to this axis with:
  //! -   scale factor S, and
  //! -   center P and creates a new axis.
    gp_Ax1 Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
  //! Applies the transformation T to this axis.
  //! and assigns the result to this axis.
    void Transform (const gp_Trsf& T);
  

  //! Applies the transformation T to this axis and creates a new one.
  //!
  //! Translates an axis plaxement in the direction of the vector
  //! <V>. The magnitude of the translation is the vector's magnitude.
    gp_Ax1 Transformed (const gp_Trsf& T) const;
  

  //! Translates this axis by the vector V,
  //! and assigns the result to this axis.
    void Translate (const gp_Vec& V);
  

  //! Translates this axis by the vector V,
  //! and creates a new one.
    gp_Ax1 Translated (const gp_Vec& V) const;
  

  //! Translates this axis by:
  //! the vector (P1, P2) defined from point P1 to point P2.
  //! and assigns the result to this axis.
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates this axis by:
  //! the vector (P1, P2) defined from point P1 to point P2.
  //! and creates a new one.
    gp_Ax1 Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Pnt loc;
  gp_Dir vdir;


};


#include <gp_Ax1.lxx>





#endif // _gp_Ax1_HeaderFile
