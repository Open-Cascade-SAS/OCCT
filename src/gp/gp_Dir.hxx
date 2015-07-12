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

#ifndef _gp_Dir_HeaderFile
#define _gp_Dir_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_DomainError;
class Standard_OutOfRange;
class gp_Vec;
class gp_XYZ;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;



//! Describes a unit vector in 3D space. This unit vector is also called "Direction".
//! See Also
//! gce_MakeDir which provides functions for more complex
//! unit vector constructions
//! Geom_Direction which provides additional functions for
//! constructing unit vectors and works, in particular, with the
//! parametric equations of unit vectors.
class gp_Dir 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a direction corresponding to X axis.
    gp_Dir();
  
  //! Normalizes the vector V and creates a direction. Raises ConstructionError if V.Magnitude() <= Resolution.
    gp_Dir(const gp_Vec& V);
  
  //! Creates a direction from a triplet of coordinates. Raises ConstructionError if Coord.Modulus() <= Resolution from gp.
    gp_Dir(const gp_XYZ& Coord);
  
  //! Creates a direction with its 3 cartesian coordinates. Raises ConstructionError if Sqrt(Xv*Xv + Yv*Yv + Zv*Zv) <= Resolution
  //! Modification of the direction's coordinates
  //! If Sqrt (X*X + Y*Y + Z*Z) <= Resolution from gp where
  //! X, Y ,Z are the new coordinates it is not possible to
  //! construct the direction and the method raises the
  //! exception ConstructionError.
    gp_Dir(const Standard_Real Xv, const Standard_Real Yv, const Standard_Real Zv);
  

  //! For this unit vector,  assigns the value Xi to:
  //! -   the X coordinate if Index is 1, or
  //! -   the Y coordinate if Index is 2, or
  //! -   the Z coordinate if Index is 3,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if Index is not 1, 2, or 3.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   Sqrt(Xv*Xv + Yv*Yv + Zv*Zv), or
  //! -   the modulus of the number triple formed by the new
  //! value Xi and the two other coordinates of this vector
  //! that were not directly modified.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! For this unit vector,  assigns the values Xv, Yv and Zv to its three coordinates.
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
    void SetCoord (const Standard_Real Xv, const Standard_Real Yv, const Standard_Real Zv);
  
  //! Assigns the given value to the X coordinate of this   unit vector.
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the Y coordinate of this   unit vector.
    void SetY (const Standard_Real Y);
  
  //! Assigns the given value to the Z  coordinate of this   unit vector.
    void SetZ (const Standard_Real Z);
  
  //! Assigns the three coordinates of Coord to this unit vector.
    void SetXYZ (const gp_XYZ& Coord);
  

  //! Returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Index = 3 => Z is returned
  //! Exceptions
  //! Standard_OutOfRange if Index is not 1, 2, or 3.
    Standard_Real Coord (const Standard_Integer Index) const;
  
  //! Returns for the  unit vector  its three coordinates Xv, Yv, and Zv.
    void Coord (Standard_Real& Xv, Standard_Real& Yv, Standard_Real& Zv) const;
  
  //! Returns the X coordinate for a  unit vector.
    Standard_Real X() const;
  
  //! Returns the Y coordinate for a  unit vector.
    Standard_Real Y() const;
  
  //! Returns the Z coordinate for a  unit vector.
    Standard_Real Z() const;
  
  //! for this unit vector, returns  its three coordinates as a number triplea.
    const gp_XYZ& XYZ() const;
  

  //! Returns True if the angle between the two directions is
  //! lower or equal to AngularTolerance.
    Standard_Boolean IsEqual (const gp_Dir& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if  the angle between this unit vector and the unit vector Other is equal to Pi/2 (normal).
    Standard_Boolean IsNormal (const gp_Dir& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if  the angle between this unit vector and the unit vector Other is equal to  Pi (opposite).
    Standard_Boolean IsOpposite (const gp_Dir& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns true if the angle between this unit vector and the
  //! unit vector Other is equal to 0 or to Pi.
  //! Note: the tolerance criterion is given by AngularTolerance.
    Standard_Boolean IsParallel (const gp_Dir& Other, const Standard_Real AngularTolerance) const;
  

  //! Computes the angular value in radians between <me> and
  //! <Other>. This value is always positive in 3D space.
  //! Returns the angle in the range [0, PI]
  Standard_EXPORT Standard_Real Angle (const gp_Dir& Other) const;
  

  //! Computes the angular value between <me> and <Other>.
  //! <VRef> is the direction of reference normal to <me> and <Other>
  //! and its orientation gives the positive sense of rotation.
  //! If the cross product <me> ^ <Other> has the same orientation
  //! as <VRef> the angular value is positive else negative.
  //! Returns the angular value in the range -PI and PI (in radians). Raises  DomainError if <me> and <Other> are not parallel this exception is raised
  //! when <VRef> is in the same plane as <me> and <Other>
  //! The tolerance criterion is Resolution from package gp.
  Standard_EXPORT Standard_Real AngleWithRef (const gp_Dir& Other, const gp_Dir& VRef) const;
  
  //! Computes the cross product between two directions
  //! Raises the exception ConstructionError if the two directions
  //! are parallel because the computed vector cannot be normalized
  //! to create a direction.
    void Cross (const gp_Dir& Right);
  void operator ^= (const gp_Dir& Right)
{
  Cross(Right);
}
  
  //! Computes the triple vector product.
  //! <me> ^ (V1 ^ V2)
  //! Raises the exception ConstructionError if V1 and V2 are parallel
  //! or <me> and (V1^V2) are parallel because the computed vector
  //! can't be normalized to create a direction.
    gp_Dir Crossed (const gp_Dir& Right) const;
  gp_Dir operator ^ (const gp_Dir& Right) const
{
  return Crossed(Right);
}
  
    void CrossCross (const gp_Dir& V1, const gp_Dir& V2);
  
  //! Computes the double vector product this ^ (V1 ^ V2).
  //! -   CrossCrossed creates a new unit vector.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! -   V1 and V2 are parallel, or
  //! -   this unit vector and (V1 ^ V2) are parallel.
  //! This is because, in these conditions, the computed vector
  //! is null and cannot be normalized.
    gp_Dir CrossCrossed (const gp_Dir& V1, const gp_Dir& V2) const;
  
  //! Computes the scalar product
    Standard_Real Dot (const gp_Dir& Other) const;
  Standard_Real operator * (const gp_Dir& Other) const
{
  return Dot(Other);
}
  

  //! Computes the triple scalar product <me> * (V1 ^ V2).
  //! Warnings :
  //! The computed vector V1' = V1 ^ V2 is not normalized
  //! to create a unitary vector. So this method never
  //! raises an exception even if V1 and V2 are parallel.
    Standard_Real DotCross (const gp_Dir& V1, const gp_Dir& V2) const;
  
    void Reverse();
  
  //! Reverses the orientation of a direction
  //! geometric transformations
  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction V which is the center of
  //! the  symmetry.]
    gp_Dir Reversed() const;
  gp_Dir operator -() const
{
  return Reversed();
}
  
  Standard_EXPORT void Mirror (const gp_Dir& V);
  

  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction V which is the center of
  //! the  symmetry.
  Standard_EXPORT gp_Dir Mirrored (const gp_Dir& V) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a direction
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT gp_Dir Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of a direction
  //! with respect to a plane. The axis placement A2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT gp_Dir Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates a direction. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Dir Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
  Standard_EXPORT void Transform (const gp_Trsf& T);
  

  //! Transforms a direction with a "Trsf" from gp.
  //! Warnings :
  //! If the scale factor of the "Trsf" T is negative then the
  //! direction <me> is reversed.
    gp_Dir Transformed (const gp_Trsf& T) const;




protected:





private:



  gp_XYZ coord;


};


#include <gp_Dir.lxx>





#endif // _gp_Dir_HeaderFile
