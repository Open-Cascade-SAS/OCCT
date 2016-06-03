// Created on: 1993-08-02
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

#ifndef _gp_Ax3_HeaderFile
#define _gp_Ax3_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class gp_Ax2;
class gp_Pnt;
class gp_Dir;
class gp_Ax1;
class gp_Trsf;
class gp_Vec;


//! Describes a coordinate system in 3D space. Unlike a
//! gp_Ax2 coordinate system, a gp_Ax3 can be
//! right-handed ("direct sense") or left-handed ("indirect sense").
//! A coordinate system is defined by:
//! -   its origin (also referred to as its "Location point"), and
//! -   three orthogonal unit vectors, termed the "X
//! Direction", the "Y Direction" and the "Direction" (also
//! referred to as the "main Direction").
//! The "Direction" of the coordinate system is called its
//! "main Direction" because whenever this unit vector is
//! modified, the "X Direction" and the "Y Direction" are
//! recomputed. However, when we modify either the "X
//! Direction" or the "Y Direction", "Direction" is not modified.
//! "Direction" is also the "Z Direction".
//! The "main Direction" is always parallel to the cross
//! product of its "X Direction" and "Y Direction".
//! If the coordinate system is right-handed, it satisfies the equation:
//! "main Direction" = "X Direction" ^ "Y Direction"
//! and if it is left-handed, it satisfies the equation:
//! "main Direction" = -"X Direction" ^ "Y Direction"
//! A coordinate system is used:
//! -   to describe geometric entities, in particular to position
//! them. The local coordinate system of a geometric
//! entity serves the same purpose as the STEP function
//! "axis placement three axes", or
//! -   to define geometric transformations.
//! Note:
//! -   We refer to the "X Axis", "Y Axis" and "Z Axis",
//! respectively, as the axes having:
//! -   the origin of the coordinate system as their origin, and
//! -   the unit vectors "X Direction", "Y Direction" and
//! "main Direction", respectively, as their unit vectors.
//! -   The "Z Axis" is also the "main Axis".
//! -   gp_Ax2 is used to define a coordinate system that must be always right-handed.
class gp_Ax3 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an object corresponding to the reference
  //! coordinate system (OXYZ).
  gp_Ax3();
  
  //! Creates  a  coordinate  system from a right-handed
  //! coordinate system.
  gp_Ax3(const gp_Ax2& A);
  
  //! Creates a  right handed axis placement with the
  //! "Location"  point  P  and  two  directions, N    gives the
  //! "Direction" and Vx gives the "XDirection".
  //! Raises ConstructionError if N and Vx are parallel (same or opposite orientation).
  gp_Ax3(const gp_Pnt& P, const gp_Dir& N, const gp_Dir& Vx);
  

  //! Creates an axis placement with the  "Location" point <P>
  //! and the normal direction <V>.
  Standard_EXPORT gp_Ax3(const gp_Pnt& P, const gp_Dir& V);
  
  //! Reverses the X direction of <me>.
  void XReverse();
  
  //! Reverses the Y direction of <me>.
  void YReverse();
  
  //! Reverses the Z direction of <me>.
  void ZReverse();
  
  //! Assigns the origin and "main Direction" of the axis A1 to
  //! this coordinate system, then recomputes its "X Direction" and "Y Direction".
  //! Note:
  //! -   The new "X Direction" is computed as follows:
  //! new "X Direction" = V1 ^(previous "X Direction" ^ V)
  //! where V is the "Direction" of A1.
  //! -   The orientation of this coordinate system
  //! (right-handed or left-handed) is not modified.
  //! Raises ConstructionError  if the "Direction" of <A1> and the "XDirection" of <me>
  //! are parallel (same or opposite orientation) because it is
  //! impossible to calculate the new "XDirection" and the new
  //! "YDirection".
  void SetAxis (const gp_Ax1& A1);
  

  //! Changes the main direction of this coordinate system,
  //! then recomputes its "X Direction" and "Y Direction".
  //! Note:
  //! -   The new "X Direction" is computed as follows:
  //! new "X Direction" = V ^ (previous "X Direction" ^ V).
  //! -   The orientation of this coordinate system (left- or right-handed) is not modified.
  //! Raises ConstructionError if <V< and the previous "XDirection" are parallel
  //! because it is impossible to calculate the new "XDirection"
  //! and the new "YDirection".
  void SetDirection (const gp_Dir& V);
  

  //! Changes the "Location" point (origin) of <me>.
  void SetLocation (const gp_Pnt& P);
  

  //! Changes the "Xdirection" of <me>. The main direction
  //! "Direction" is not modified, the "Ydirection" is modified.
  //! If <Vx> is not normal to the main direction then <XDirection>
  //! is computed as follows XDirection = Direction ^ (Vx ^ Direction).
  //! Raises ConstructionError if <Vx> is parallel (same or opposite
  //! orientation) to the main direction of <me>
  void SetXDirection (const gp_Dir& Vx);
  

  //! Changes the "Ydirection" of <me>. The main direction is not
  //! modified but the "Xdirection" is changed.
  //! If <Vy> is not normal to the main direction then "YDirection"
  //! is computed as  follows
  //! YDirection = Direction ^ (<Vy> ^ Direction).
  //! Raises ConstructionError if <Vy> is parallel to the main direction of <me>
    void SetYDirection (const gp_Dir& Vy);
  

  //! Computes the angular value between the main direction of
  //! <me> and the main direction of <Other>. Returns the angle
  //! between 0 and PI in radians.
    Standard_Real Angle (const gp_Ax3& Other) const;
  

  //! Returns the main axis of <me>. It is the "Location" point
  //! and the main "Direction".
    const gp_Ax1& Axis() const;
  
  //! Computes a right-handed coordinate system with the
  //! same "X Direction" and "Y Direction" as those of this
  //! coordinate system, then recomputes the "main Direction".
  //! If this coordinate system is right-handed, the result
  //! returned is the same coordinate system. If this
  //! coordinate system is left-handed, the result is reversed.
  gp_Ax2 Ax2() const;
  

  //! Returns the main direction of <me>.
    const gp_Dir& Direction() const;
  

  //! Returns the "Location" point (origin) of <me>.
    const gp_Pnt& Location() const;
  

  //! Returns the "XDirection" of <me>.
    const gp_Dir& XDirection() const;
  

  //! Returns the "YDirection" of <me>.
    const gp_Dir& YDirection() const;
  
  //! Returns  True if  the  coordinate  system is right-handed. i.e.
  //! XDirection().Crossed(YDirection()).Dot(Direction()) > 0
    Standard_Boolean Direct() const;
  

  //! Returns True if
  //! . the distance between the "Location" point of <me> and
  //! <Other> is lower or equal to LinearTolerance and
  //! . the distance between the "Location" point of <Other> and
  //! <me> is lower or equal to LinearTolerance and
  //! . the main direction of <me> and the main direction of
  //! <Other> are parallel (same or opposite orientation).
    Standard_Boolean IsCoplanar (const gp_Ax3& Other, const Standard_Real LinearTolerance, const Standard_Real AngularTolerance) const;
  
  //! Returns True if
  //! . the distance between <me> and the "Location" point of A1
  //! is lower of equal to LinearTolerance and
  //! . the distance between A1 and the "Location" point of <me>
  //! is lower or equal to LinearTolerance and
  //! . the main direction of <me> and the direction of A1 are normal.
    Standard_Boolean IsCoplanar (const gp_Ax1& A1, const Standard_Real LinearTolerance, const Standard_Real AngularTolerance) const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point P which is the
  //! center of the symmetry.
  //! Warnings :
  //! The main direction of the axis placement is not changed.
  //! The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  Standard_EXPORT gp_Ax3 Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry.
  //! The transformation is performed on the "Location"
  //! point, on the "XDirection" and "YDirection".
  //! The resulting main "Direction" is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_EXPORT gp_Ax3 Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to a plane.
  //! The axis placement  <A2> locates the plane of the symmetry :
  //! (Location, XDirection, YDirection).
  //! The transformation is performed on the "Location"
  //! point, on the "XDirection" and "YDirection".
  //! The resulting main "Direction" is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_EXPORT gp_Ax3 Mirrored (const gp_Ax2& A2) const;
  
  void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates an axis placement. <A1> is the axis of the
  //! rotation . Ang is the angular value of the rotation
  //! in radians.
  gp_Ax3 Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
  void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Applies a scaling transformation on the axis placement.
  //! The "Location" point of the axisplacement is modified.
  //! Warnings :
  //! If the scale <S> is negative :
  //! . the main direction of the axis placement is not changed.
  //! . The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  gp_Ax3 Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
  void Transform (const gp_Trsf& T);
  

  //! Transforms an axis placement with a Trsf.
  //! The "Location" point, the "XDirection" and the
  //! "YDirection" are transformed with T.  The resulting
  //! main "Direction" of <me> is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  gp_Ax3 Transformed (const gp_Trsf& T) const;
  
  void Translate (const gp_Vec& V);
  

  //! Translates an axis plaxement in the direction of the vector
  //! <V>. The magnitude of the translation is the vector's magnitude.
  gp_Ax3 Translated (const gp_Vec& V) const;
  
  void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates an axis placement from the point <P1> to the
  //! point <P2>.
  gp_Ax3 Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax1 axis;
  gp_Dir vydir;
  gp_Dir vxdir;


};


#include <gp_Ax3.lxx>





#endif // _gp_Ax3_HeaderFile
