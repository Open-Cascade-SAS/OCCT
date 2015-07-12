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

#ifndef _gp_Dir2d_HeaderFile
#define _gp_Dir2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XY.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_DomainError;
class Standard_OutOfRange;
class gp_Vec2d;
class gp_XY;
class gp_Ax2d;
class gp_Trsf2d;


//! Describes a unit vector in the plane (2D space). This unit
//! vector is also called "Direction".
//! See Also
//! gce_MakeDir2d which provides functions for more
//! complex unit vector constructions
//! Geom2d_Direction which provides additional functions
//! for constructing unit vectors and works, in particular, with
//! the parametric equations of unit vectors
class gp_Dir2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a direction corresponding to X axis.
    gp_Dir2d();
  
  //! Normalizes the vector V and creates a Direction. Raises ConstructionError if V.Magnitude() <= Resolution from gp.
    gp_Dir2d(const gp_Vec2d& V);
  
  //! Creates a Direction from a doublet of coordinates. Raises ConstructionError if Coord.Modulus() <= Resolution from gp.
    gp_Dir2d(const gp_XY& Coord);
  
  //! Creates a Direction with its 2 cartesian coordinates. Raises ConstructionError if Sqrt(Xv*Xv + Yv*Yv) <= Resolution from gp.
    gp_Dir2d(const Standard_Real Xv, const Standard_Real Yv);
  

  //! For this unit vector, assigns:
  //! the value Xi to:
  //! -   the X coordinate if Index is 1, or
  //! -   the Y coordinate if Index is 2, and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if Index is not 1 or 2.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   Sqrt(Xv*Xv + Yv*Yv), or
  //! -   the modulus of the number pair formed by the new
  //! value Xi and the other coordinate of this vector that
  //! was not directly modified.
  //! Raises OutOfRange if Index != {1, 2}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  

  //! For this unit vector, assigns:
  //! -   the values Xv and Yv to its two coordinates,
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if Index is not 1 or 2.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   Sqrt(Xv*Xv + Yv*Yv), or
  //! -   the modulus of the number pair formed by the new
  //! value Xi and the other coordinate of this vector that
  //! was not directly modified.
  //! Raises OutOfRange if Index != {1, 2}.
    void SetCoord (const Standard_Real Xv, const Standard_Real Yv);
  

  //! Assigns the given value to the X coordinate of this unit   vector,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   the modulus of Coord, or
  //! -   the modulus of the number pair formed from the new
  //! X or Y coordinate and the other coordinate of this
  //! vector that was not directly modified.
    void SetX (const Standard_Real X);
  

  //! Assigns  the given value to the Y coordinate of this unit   vector,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   the modulus of Coord, or
  //! -   the modulus of the number pair formed from the new
  //! X or Y coordinate and the other coordinate of this
  //! vector that was not directly modified.
    void SetY (const Standard_Real Y);
  

  //! Assigns:
  //! -   the two coordinates of Coord to this unit vector,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   the modulus of Coord, or
  //! -   the modulus of the number pair formed from the new
  //! X or Y coordinate and the other coordinate of this
  //! vector that was not directly modified.
    void SetXY (const gp_XY& Coord);
  

  //! For this unit vector returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Raises OutOfRange if Index != {1, 2}.
    Standard_Real Coord (const Standard_Integer Index) const;
  
  //! For this unit vector returns its two coordinates Xv and Yv.
  //! Raises OutOfRange if Index != {1, 2}.
    void Coord (Standard_Real& Xv, Standard_Real& Yv) const;
  
  //! For this unit vector, returns its X coordinate.
    Standard_Real X() const;
  
  //! For this unit vector, returns its Y coordinate.
    Standard_Real Y() const;
  
  //! For this unit vector, returns its two coordinates as a number pair.
  //! Comparison between Directions
  //! The precision value is an input data.
    const gp_XY& XY() const;
  

  //! Returns True if the two vectors have the same direction
  //! i.e. the angle between this unit vector and the
  //! unit vector Other is less than or equal to AngularTolerance.
    Standard_Boolean IsEqual (const gp_Dir2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if the angle between this unit vector and the
  //! unit vector Other is equal to Pi/2 or -Pi/2 (normal)
  //! i.e. Abs(Abs(<me>.Angle(Other)) - PI/2.) <= AngularTolerance
    Standard_Boolean IsNormal (const gp_Dir2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if the angle between this unit vector and the
  //! unit vector Other is equal to Pi or -Pi (opposite).
  //! i.e.  PI - Abs(<me>.Angle(Other)) <= AngularTolerance
    Standard_Boolean IsOpposite (const gp_Dir2d& Other, const Standard_Real AngularTolerance) const;
  

  //! returns true if if the angle between this unit vector and unit
  //! vector Other is equal to 0, Pi or -Pi.
  //! i.e.  Abs(Angle(<me>, Other)) <= AngularTolerance or
  //! PI - Abs(Angle(<me>, Other)) <= AngularTolerance
    Standard_Boolean IsParallel (const gp_Dir2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Computes the angular value in radians between <me> and
  //! <Other>. Returns the angle in the range [-PI, PI].
  Standard_EXPORT Standard_Real Angle (const gp_Dir2d& Other) const;
  

  //! Computes the cross product between two directions.
    Standard_Real Crossed (const gp_Dir2d& Right) const;
  Standard_Real operator ^ (const gp_Dir2d& Right) const
{
  return Crossed(Right);
}
  
  //! Computes the scalar product
    Standard_Real Dot (const gp_Dir2d& Other) const;
  Standard_Real operator * (const gp_Dir2d& Other) const
{
  return Dot(Other);
}
  
    void Reverse();
  
  //! Reverses the orientation of a direction
    gp_Dir2d Reversed() const;
  gp_Dir2d operator -() const
{
  return Reversed();
}
  
  Standard_EXPORT void Mirror (const gp_Dir2d& V);
  

  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction V which is the center of
  //! the  symmetry.
  Standard_EXPORT gp_Dir2d Mirrored (const gp_Dir2d& V) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2d& A);
  

  //! Performs the symmetrical transformation of a direction
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT gp_Dir2d Mirrored (const gp_Ax2d& A) const;
  
    void Rotate (const Standard_Real Ang);
  

  //! Rotates a direction.  Ang is the angular value of
  //! the rotation in radians.
    gp_Dir2d Rotated (const Standard_Real Ang) const;
  
  Standard_EXPORT void Transform (const gp_Trsf2d& T);
  

  //! Transforms a direction with the "Trsf" T.
  //! Warnings :
  //! If the scale factor of the "Trsf" T is negative then the
  //! direction <me> is reversed.
    gp_Dir2d Transformed (const gp_Trsf2d& T) const;




protected:





private:



  gp_XY coord;


};


#include <gp_Dir2d.lxx>





#endif // _gp_Dir2d_HeaderFile
