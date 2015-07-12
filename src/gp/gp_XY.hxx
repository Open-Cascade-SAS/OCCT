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

#ifndef _gp_XY_HeaderFile
#define _gp_XY_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Mat2d;



//! This class describes a cartesian coordinate entity in 2D
//! space {X,Y}. This class is non persistent. This entity used
//! for algebraic calculation. An XY can be transformed with a
//! Trsf2d or a  GTrsf2d from package gp.
//! It is used in vectorial computations or for holding this type
//! of information in data structures.
class gp_XY 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates XY object with zero coordinates (0,0).
    gp_XY();
  
  //! a number pair defined by the XY coordinates
    gp_XY(const Standard_Real X, const Standard_Real Y);
  

  //! modifies the coordinate of range Index
  //! Index = 1 => X is modified
  //! Index = 2 => Y is modified
  //! Raises OutOfRange if Index != {1, 2}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! For this number pair, assigns
  //! the values X and Y to its coordinates
    void SetCoord (const Standard_Real X, const Standard_Real Y);
  
  //! Assigns the given value to the X coordinate of this number pair.
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the Y  coordinate of this number pair.
    void SetY (const Standard_Real Y);
  

  //! returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Raises OutOfRange if Index != {1, 2}.
    Standard_Real Coord (const Standard_Integer Index) const;
  
    Standard_Real& ChangeCoord (const Standard_Integer theIndex);
  
  //! For this number pair, returns its coordinates X and Y.
    void Coord (Standard_Real& X, Standard_Real& Y) const;
  
  //! Returns the X coordinate of this number pair.
    Standard_Real X() const;
  
  //! Returns the Y coordinate of this number pair.
    Standard_Real Y() const;
  
  //! Computes Sqrt (X*X + Y*Y) where X and Y are the two coordinates of this number pair.
    Standard_Real Modulus() const;
  
  //! Computes X*X + Y*Y where X and Y are the two coordinates of this number pair.
    Standard_Real SquareModulus() const;
  

  //! Returns true if the coordinates of this number pair are
  //! equal to the respective coordinates of the number pair
  //! Other, within the specified tolerance Tolerance. I.e.:
  //! abs(<me>.X() - Other.X()) <= Tolerance and
  //! abs(<me>.Y() - Other.Y()) <= Tolerance and
  //! computations
  Standard_EXPORT Standard_Boolean IsEqual (const gp_XY& Other, const Standard_Real Tolerance) const;
  
  //! Computes the sum of this number pair and number pair Other
  //! <me>.X() = <me>.X() + Other.X()
  //! <me>.Y() = <me>.Y() + Other.Y()
    void Add (const gp_XY& Other);
  void operator += (const gp_XY& Other)
{
  Add(Other);
}
  
  //! Computes the sum of this number pair and number pair Other
  //! new.X() = <me>.X() + Other.X()
  //! new.Y() = <me>.Y() + Other.Y()
    gp_XY Added (const gp_XY& Other) const;
  gp_XY operator + (const gp_XY& Other) const
{
  return Added(Other);
}
  

  //! Real D = <me>.X() * Other.Y() - <me>.Y() * Other.X()
    Standard_Real Crossed (const gp_XY& Right) const;
  Standard_Real operator ^ (const gp_XY& Right) const
{
  return Crossed(Right);
}
  

  //! computes the magnitude of the cross product between <me> and
  //! Right. Returns || <me> ^ Right ||
    Standard_Real CrossMagnitude (const gp_XY& Right) const;
  

  //! computes the square magnitude of the cross product between <me> and
  //! Right. Returns || <me> ^ Right ||**2
    Standard_Real CrossSquareMagnitude (const gp_XY& Right) const;
  
  //! divides <me> by a real.
    void Divide (const Standard_Real Scalar);
  void operator /= (const Standard_Real Scalar)
{
  Divide(Scalar);
}
  
  //! Divides <me> by a real.
    gp_XY Divided (const Standard_Real Scalar) const;
  gp_XY operator / (const Standard_Real Scalar) const
{
  return Divided(Scalar);
}
  
  //! Computes the scalar product between <me> and Other
    Standard_Real Dot (const gp_XY& Other) const;
  Standard_Real operator * (const gp_XY& Other) const
{
  return Dot(Other);
}
  

  //! <me>.X() = <me>.X() * Scalar;
  //! <me>.Y() = <me>.Y() * Scalar;
    void Multiply (const Standard_Real Scalar);
  void operator *= (const Standard_Real Scalar)
{
  Multiply(Scalar);
}
  

  //! <me>.X() = <me>.X() * Other.X();
  //! <me>.Y() = <me>.Y() * Other.Y();
    void Multiply (const gp_XY& Other);
  void operator *= (const gp_XY& Other)
{
  Multiply(Other);
}
  
  //! <me> = Matrix * <me>
    void Multiply (const gp_Mat2d& Matrix);
  void operator *= (const gp_Mat2d& Matrix)
{
  Multiply(Matrix);
}
  

  //! New.X() = <me>.X() * Scalar;
  //! New.Y() = <me>.Y() * Scalar;
    gp_XY Multiplied (const Standard_Real Scalar) const;
  gp_XY operator * (const Standard_Real Scalar) const
{
  return Multiplied(Scalar);
}
  

  //! new.X() = <me>.X() * Other.X();
  //! new.Y() = <me>.Y() * Other.Y();
    gp_XY Multiplied (const gp_XY& Other) const;
  
  //! New = Matrix * <me>
    gp_XY Multiplied (const gp_Mat2d& Matrix) const;
  gp_XY operator * (const gp_Mat2d& Matrix) const
{
  return Multiplied(Matrix);
}
  

  //! <me>.X() = <me>.X()/ <me>.Modulus()
  //! <me>.Y() = <me>.Y()/ <me>.Modulus()
  //! Raises ConstructionError if <me>.Modulus() <= Resolution from gp
    void Normalize();
  

  //! New.X() = <me>.X()/ <me>.Modulus()
  //! New.Y() = <me>.Y()/ <me>.Modulus()
  //! Raises ConstructionError if <me>.Modulus() <= Resolution from gp
    gp_XY Normalized() const;
  

  //! <me>.X() = -<me>.X()
  //! <me>.Y() = -<me>.Y()
    void Reverse();
  

  //! New.X() = -<me>.X()
  //! New.Y() = -<me>.Y()
    gp_XY Reversed() const;
  gp_XY operator -() const
{
  return Reversed();
}
  

  //! Computes  the following linear combination and
  //! assigns the result to this number pair:
  //! A1 * XY1 + A2 * XY2
    void SetLinearForm (const Standard_Real A1, const gp_XY& XY1, const Standard_Real A2, const gp_XY& XY2);
  

  //! --  Computes  the following linear combination and
  //! assigns the result to this number pair:
  //! A1 * XY1 + A2 * XY2 + XY3
    void SetLinearForm (const Standard_Real A1, const gp_XY& XY1, const Standard_Real A2, const gp_XY& XY2, const gp_XY& XY3);
  

  //! Computes  the following linear combination and
  //! assigns the result to this number pair:
  //! A1 * XY1 + XY2
    void SetLinearForm (const Standard_Real A1, const gp_XY& XY1, const gp_XY& XY2);
  

  //! Computes  the following linear combination and
  //! assigns the result to this number pair:
  //! XY1 + XY2
    void SetLinearForm (const gp_XY& XY1, const gp_XY& XY2);
  

  //! <me>.X() = <me>.X() - Other.X()
  //! <me>.Y() = <me>.Y() - Other.Y()
    void Subtract (const gp_XY& Right);
  void operator -= (const gp_XY& Right)
{
  Subtract(Right);
}
  

  //! new.X() = <me>.X() - Other.X()
  //! new.Y() = <me>.Y() - Other.Y()
    gp_XY Subtracted (const gp_XY& Right) const;
  gp_XY operator - (const gp_XY& Right) const
{
  return Subtracted(Right);
}




protected:





private:



  Standard_Real x;
  Standard_Real y;


};


#include <gp_XY.lxx>





#endif // _gp_XY_HeaderFile
