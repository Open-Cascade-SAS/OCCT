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

#ifndef _gp_Vec2d_HeaderFile
#define _gp_Vec2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XY.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_VectorWithNullMagnitude;
class gp_Dir2d;
class gp_XY;
class gp_Pnt2d;
class gp_Ax2d;
class gp_Trsf2d;



//! Defines a non-persistent vector in 2D space.
class gp_Vec2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a zero vector.
    gp_Vec2d();
  
  //! Creates a unitary vector from a direction V.
    gp_Vec2d(const gp_Dir2d& V);
  
  //! Creates a vector with a doublet of coordinates.
    gp_Vec2d(const gp_XY& Coord);
  
  //! Creates a point with its two Cartesian coordinates.
    gp_Vec2d(const Standard_Real Xv, const Standard_Real Yv);
  

  //! Creates a vector from two points. The length of the vector
  //! is the distance between P1 and P2
    gp_Vec2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  //! Changes the coordinate of range Index
  //! Index = 1 => X is modified
  //! Index = 2 => Y is modified
  //! Raises OutOfRange if Index != {1, 2}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! For this vector, assigns
  //! the values Xv and Yv to its two coordinates
    void SetCoord (const Standard_Real Xv, const Standard_Real Yv);
  
  //! Assigns the given value to the X coordinate of this vector.
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the Y coordinate of this vector.
    void SetY (const Standard_Real Y);
  
  //! Assigns the two coordinates of Coord to this vector.
    void SetXY (const gp_XY& Coord);
  

  //! Returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Raised if Index != {1, 2}.
    Standard_Real Coord (const Standard_Integer Index) const;
  
  //! For this vector, returns  its two coordinates Xv and Yv
    void Coord (Standard_Real& Xv, Standard_Real& Yv) const;
  
  //! For this vector, returns its X  coordinate.
    Standard_Real X() const;
  
  //! For this vector, returns its Y  coordinate.
    Standard_Real Y() const;
  
  //! For this vector, returns its two coordinates as a number pair
    const gp_XY& XY() const;
  

  //! Returns True if the two vectors have the same magnitude value
  //! and the same direction. The precision values are LinearTolerance
  //! for the magnitude and AngularTolerance for the direction.
  Standard_EXPORT Standard_Boolean IsEqual (const gp_Vec2d& Other, const Standard_Real LinearTolerance, const Standard_Real AngularTolerance) const;
  

  //! Returns True if abs(Abs(<me>.Angle(Other)) - PI/2.)
  //! <= AngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp.
    Standard_Boolean IsNormal (const gp_Vec2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if PI - Abs(<me>.Angle(Other)) <= AngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp.
  Standard_Boolean IsOpposite (const gp_Vec2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns true if Abs(Angle(<me>, Other)) <= AngularTolerance or
  //! PI - Abs(Angle(<me>, Other)) <= AngularTolerance
  //! Two vectors with opposite directions are considered as parallel.
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp
  Standard_Boolean IsParallel (const gp_Vec2d& Other, const Standard_Real AngularTolerance) const;
  

  //! Computes the angular value between <me> and <Other>
  //! returns the angle value between -PI and PI in radian.
  //! The orientation is from <me> to Other. The positive sense is the
  //! trigonometric sense.
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution from gp or
  //! Other.Magnitude() <= Resolution because the angular value is
  //! indefinite if one of the vectors has a null magnitude.
  Standard_EXPORT Standard_Real Angle (const gp_Vec2d& Other) const;
  
  //! Computes the magnitude of this vector.
    Standard_Real Magnitude() const;
  
  //! Computes the square magnitude of this vector.
    Standard_Real SquareMagnitude() const;
  
    void Add (const gp_Vec2d& Other);
    void operator += (const gp_Vec2d& Other)
    {
      Add(Other);
    }
  
  //! Adds two vectors
    gp_Vec2d Added (const gp_Vec2d& Other) const;
    gp_Vec2d operator + (const gp_Vec2d& Other) const
    {
      return Added(Other);
    }
  
  //! Computes the crossing product between two vectors
    Standard_Real Crossed (const gp_Vec2d& Right) const;
    Standard_Real operator ^ (const gp_Vec2d& Right) const
    {
      return Crossed(Right);
    }
  

  //! Computes the magnitude of the cross product between <me> and
  //! Right. Returns || <me> ^ Right ||
    Standard_Real CrossMagnitude (const gp_Vec2d& Right) const;
  

  //! Computes the square magnitude of the cross product between <me> and
  //! Right. Returns || <me> ^ Right ||**2
    Standard_Real CrossSquareMagnitude (const gp_Vec2d& Right) const;
  
    void Divide (const Standard_Real Scalar);
    void operator /= (const Standard_Real Scalar)
    {
      Divide(Scalar);
    }
  
  //! divides a vector by a scalar
    gp_Vec2d Divided (const Standard_Real Scalar) const;
    gp_Vec2d operator / (const Standard_Real Scalar) const
    {
      return Divided(Scalar);
    }
  
  //! Computes the scalar product
    Standard_Real Dot (const gp_Vec2d& Other) const;
    Standard_Real operator * (const gp_Vec2d& Other) const
    {
      return Dot(Other);
    }

    gp_Vec2d GetNormal() const;
  
    void Multiply (const Standard_Real Scalar);
    void operator *= (const Standard_Real Scalar)
    {
      Multiply(Scalar);
    }
  
  //! Normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from package gp.
    gp_Vec2d Multiplied (const Standard_Real Scalar) const;
    gp_Vec2d operator * (const Standard_Real Scalar) const
    {
      return Multiplied(Scalar);
    }
  
    void Normalize();
  
  //! Normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from package gp.
  //! Reverses the direction of a vector
    gp_Vec2d Normalized() const;
  
    void Reverse();
  
  //! Reverses the direction of a vector
    gp_Vec2d Reversed() const;
    gp_Vec2d operator -() const
    {
      return Reversed();
    }
  
  //! Subtracts two vectors
    void Subtract (const gp_Vec2d& Right);
    void operator -= (const gp_Vec2d& Right)
    {
      Subtract(Right);
    }
  
  //! Subtracts two vectors
    gp_Vec2d Subtracted (const gp_Vec2d& Right) const;
    gp_Vec2d operator - (const gp_Vec2d& Right) const
    {
      return Subtracted(Right);
    }
  

  //! <me> is set to the following linear form :
  //! A1 * V1 + A2 * V2 + V3
    void SetLinearForm (const Standard_Real A1, const gp_Vec2d& V1, const Standard_Real A2, const gp_Vec2d& V2, const gp_Vec2d& V3);
  

  //! <me> is set to the following linear form : A1 * V1 + A2 * V2
    void SetLinearForm (const Standard_Real A1, const gp_Vec2d& V1, const Standard_Real A2, const gp_Vec2d& V2);
  

  //! <me> is set to the following linear form : A1 * V1 + V2
    void SetLinearForm (const Standard_Real A1, const gp_Vec2d& V1, const gp_Vec2d& V2);
  

  //! <me> is set to the following linear form : Left + Right
    void SetLinearForm (const gp_Vec2d& Left, const gp_Vec2d& Right);
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to the vector V which is the center of
  //! the  symmetry.
  Standard_EXPORT void Mirror (const gp_Vec2d& V);
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to the vector V which is the center of
  //! the  symmetry.
  Standard_EXPORT gp_Vec2d Mirrored (const gp_Vec2d& V) const;
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT void Mirror (const gp_Ax2d& A1);
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT gp_Vec2d Mirrored (const gp_Ax2d& A1) const;
  
    void Rotate (const Standard_Real Ang);
  

  //! Rotates a vector. Ang is the angular value of the
  //! rotation in radians.
    gp_Vec2d Rotated (const Standard_Real Ang) const;
  
    void Scale (const Standard_Real S);
  
  //! Scales a vector. S is the scaling value.
    gp_Vec2d Scaled (const Standard_Real S) const;
  
  Standard_EXPORT void Transform (const gp_Trsf2d& T);
  
  //! Transforms a vector with a Trsf from gp.
    gp_Vec2d Transformed (const gp_Trsf2d& T) const;




protected:





private:



  gp_XY coord;


};


#include <gp_Vec2d.lxx>





#endif // _gp_Vec2d_HeaderFile
