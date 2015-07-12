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

#ifndef _gp_Vec_HeaderFile
#define _gp_Vec_HeaderFile

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
class gp_VectorWithNullMagnitude;
class gp_Dir;
class gp_XYZ;
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;



//! Defines a non-persistent vector in 3D space.
class gp_Vec 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a zero vector.
    gp_Vec();
  
  //! Creates a unitary vector from a direction V.
    gp_Vec(const gp_Dir& V);
  
  //! Creates a vector with a triplet of coordinates.
    gp_Vec(const gp_XYZ& Coord);
  
  //! Creates a point with its three cartesian coordinates.
    gp_Vec(const Standard_Real Xv, const Standard_Real Yv, const Standard_Real Zv);
  

  //! Creates a vector from two points. The length of the vector
  //! is the distance between P1 and P2
    gp_Vec(const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Changes the coordinate of range Index
  //! Index = 1 => X is modified
  //! Index = 2 => Y is modified
  //! Index = 3 => Z is modified
  //! Raised if Index != {1, 2, 3}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! For this vector, assigns
  //! -   the values Xv, Yv and Zv to its three coordinates.
    void SetCoord (const Standard_Real Xv, const Standard_Real Yv, const Standard_Real Zv);
  
  //! Assigns the given value to the X coordinate of this vector.
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the X coordinate of this vector.
    void SetY (const Standard_Real Y);
  
  //! Assigns the given value to the X coordinate of this vector.
    void SetZ (const Standard_Real Z);
  
  //! Assigns the three coordinates of Coord to this vector.
    void SetXYZ (const gp_XYZ& Coord);
  

  //! Returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Index = 3 => Z is returned
  //! Raised if Index != {1, 2, 3}.
    Standard_Real Coord (const Standard_Integer Index) const;
  
  //! For this vector returns its three coordinates Xv, Yv, and Zvinline
    void Coord (Standard_Real& Xv, Standard_Real& Yv, Standard_Real& Zv) const;
  
  //! For this vector, returns its X coordinate.
    Standard_Real X() const;
  
  //! For this vector, returns its Y coordinate.
    Standard_Real Y() const;
  
  //! For this vector, returns its Z  coordinate.
    Standard_Real Z() const;
  
  //! For this vector, returns
  //! -   its three coordinates as a number triple
    const gp_XYZ& XYZ() const;
  

  //! Returns True if the two vectors have the same magnitude value
  //! and the same direction. The precision values are LinearTolerance
  //! for the magnitude and AngularTolerance for the direction.
  Standard_EXPORT Standard_Boolean IsEqual (const gp_Vec& Other, const Standard_Real LinearTolerance, const Standard_Real AngularTolerance) const;
  

  //! Returns True if abs(<me>.Angle(Other) - PI/2.) <= AngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp
    Standard_Boolean IsNormal (const gp_Vec& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if PI - <me>.Angle(Other) <= AngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp
    Standard_Boolean IsOpposite (const gp_Vec& Other, const Standard_Real AngularTolerance) const;
  

  //! Returns True if Angle(<me>, Other) <= AngularTolerance or
  //! PI - Angle(<me>, Other) <= AngularTolerance
  //! This definition means that two parallel vectors cannot define
  //! a plane but two vectors with opposite directions are considered
  //! as parallel. Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp
    Standard_Boolean IsParallel (const gp_Vec& Other, const Standard_Real AngularTolerance) const;
  

  //! Computes the angular value between <me> and <Other>
  //! Returns the angle value between 0 and PI in radian.
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution from gp or
  //! Other.Magnitude() <= Resolution because the angular value is
  //! indefinite if one of the vectors has a null magnitude.
    Standard_Real Angle (const gp_Vec& Other) const;
  
  //! Computes the angle, in radians, between this vector and
  //! vector Other. The result is a value between -Pi and Pi.
  //! For this, VRef defines the positive sense of rotation: the
  //! angular value is positive, if the cross product this ^ Other
  //! has the same orientation as VRef relative to the plane
  //! defined by the vectors this and Other. Otherwise, the
  //! angular value is negative.
  //! Exceptions
  //! gp_VectorWithNullMagnitude if the magnitude of this
  //! vector, the vector Other, or the vector VRef is less than or
  //! equal to gp::Resolution().
  //! Standard_DomainError if this vector, the vector Other,
  //! and the vector VRef are coplanar, unless this vector and
  //! the vector Other are parallel.
    Standard_Real AngleWithRef (const gp_Vec& Other, const gp_Vec& VRef) const;
  
  //! Computes the magnitude of this vector.
    Standard_Real Magnitude() const;
  
  //! Computes the square magnitude of this vector.
    Standard_Real SquareMagnitude() const;
  
  //! Adds two vectors
    void Add (const gp_Vec& Other);
  void operator += (const gp_Vec& Other)
{
  Add(Other);
}
  
  //! Adds two vectors
    gp_Vec Added (const gp_Vec& Other) const;
  gp_Vec operator + (const gp_Vec& Other) const
{
  return Added(Other);
}
  
  //! Subtracts two vectors
    void Subtract (const gp_Vec& Right);
  void operator -= (const gp_Vec& Right)
{
  Subtract(Right);
}
  
  //! Subtracts two vectors
    gp_Vec Subtracted (const gp_Vec& Right) const;
  gp_Vec operator - (const gp_Vec& Right) const
{
  return Subtracted(Right);
}
  
  //! Multiplies a vector by a scalar
    void Multiply (const Standard_Real Scalar);
  void operator *= (const Standard_Real Scalar)
{
  Multiply(Scalar);
}
  
  //! Multiplies a vector by a scalar
    gp_Vec Multiplied (const Standard_Real Scalar) const;
  gp_Vec operator * (const Standard_Real Scalar) const
{
  return Multiplied(Scalar);
}
  
  //! Divides a vector by a scalar
    void Divide (const Standard_Real Scalar);
  void operator /= (const Standard_Real Scalar)
{
  Divide(Scalar);
}
  
  //! Divides a vector by a scalar
    gp_Vec Divided (const Standard_Real Scalar) const;
  gp_Vec operator / (const Standard_Real Scalar) const
{
  return Divided(Scalar);
}
  
  //! computes the cross product between two vectors
    void Cross (const gp_Vec& Right);
  void operator ^= (const gp_Vec& Right)
{
  Cross(Right);
}
  
  //! computes the cross product between two vectors
    gp_Vec Crossed (const gp_Vec& Right) const;
  gp_Vec operator ^ (const gp_Vec& Right) const
{
  return Crossed(Right);
}
  

  //! Computes the magnitude of the cross
  //! product between <me> and Right.
  //! Returns || <me> ^ Right ||
    Standard_Real CrossMagnitude (const gp_Vec& Right) const;
  

  //! Computes the square magnitude of
  //! the cross product between <me> and Right.
  //! Returns || <me> ^ Right ||**2
    Standard_Real CrossSquareMagnitude (const gp_Vec& Right) const;
  
  //! Computes the triple vector product.
  //! <me> ^= (V1 ^ V2)
    void CrossCross (const gp_Vec& V1, const gp_Vec& V2);
  
  //! Computes the triple vector product.
  //! <me> ^ (V1 ^ V2)
    gp_Vec CrossCrossed (const gp_Vec& V1, const gp_Vec& V2) const;
  
  //! computes the scalar product
    Standard_Real Dot (const gp_Vec& Other) const;
  Standard_Real operator * (const gp_Vec& Other) const
{
  return Dot(Other);
}
  
  //! Computes the triple scalar product <me> * (V1 ^ V2).
    Standard_Real DotCross (const gp_Vec& V1, const gp_Vec& V2) const;
  
  //! normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from gp.
    void Normalize();
  
  //! normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from gp.
    gp_Vec Normalized() const;
  
  //! Reverses the direction of a vector
    void Reverse();
  
  //! Reverses the direction of a vector
    gp_Vec Reversed() const;
  gp_Vec operator -() const
{
  return Reversed();
}
  

  //! <me> is set to the following linear form :
  //! A1 * V1 + A2 * V2 + A3 * V3 + V4
    void SetLinearForm (const Standard_Real A1, const gp_Vec& V1, const Standard_Real A2, const gp_Vec& V2, const Standard_Real A3, const gp_Vec& V3, const gp_Vec& V4);
  

  //! <me> is set to the following linear form :
  //! A1 * V1 + A2 * V2 + A3 * V3
    void SetLinearForm (const Standard_Real A1, const gp_Vec& V1, const Standard_Real A2, const gp_Vec& V2, const Standard_Real A3, const gp_Vec& V3);
  

  //! <me> is set to the following linear form :
  //! A1 * V1 + A2 * V2 + V3
    void SetLinearForm (const Standard_Real A1, const gp_Vec& V1, const Standard_Real A2, const gp_Vec& V2, const gp_Vec& V3);
  

  //! <me> is set to the following linear form :
  //! A1 * V1 + A2 * V2
    void SetLinearForm (const Standard_Real A1, const gp_Vec& V1, const Standard_Real A2, const gp_Vec& V2);
  

  //! <me> is set to the following linear form : A1 * V1 + V2
    void SetLinearForm (const Standard_Real A1, const gp_Vec& V1, const gp_Vec& V2);
  

  //! <me> is set to the following linear form : V1 + V2
    void SetLinearForm (const gp_Vec& V1, const gp_Vec& V2);
  
  Standard_EXPORT void Mirror (const gp_Vec& V);
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to the vector V which is the center of
  //! the  symmetry.
  Standard_EXPORT gp_Vec Mirrored (const gp_Vec& V) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT gp_Vec Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of a vector
  //! with respect to a plane. The axis placement A2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT gp_Vec Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates a vector. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Vec Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const Standard_Real S);
  
  //! Scales a vector. S is the scaling value.
    gp_Vec Scaled (const Standard_Real S) const;
  
  //! Transforms a vector with the transformation T.
  Standard_EXPORT void Transform (const gp_Trsf& T);
  
  //! Transforms a vector with the transformation T.
    gp_Vec Transformed (const gp_Trsf& T) const;




protected:





private:



  gp_XYZ coord;


};


#include <gp_Vec.lxx>





#endif // _gp_Vec_HeaderFile
