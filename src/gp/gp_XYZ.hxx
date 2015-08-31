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

#ifndef _gp_XYZ_HeaderFile
#define _gp_XYZ_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Mat;



//! This class describes a cartesian coordinate entity in
//! 3D space {X,Y,Z}. This entity is used for algebraic
//! calculation. This entity can be transformed
//! with a "Trsf" or a  "GTrsf" from package "gp".
//! It is used in vectorial computations or for holding this type
//! of information in data structures.
class gp_XYZ 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an XYZ object with zero co-ordinates (0,0,0)
    gp_XYZ();
  
  //! creates an XYZ with given coordinates
    gp_XYZ(const Standard_Real X, const Standard_Real Y, const Standard_Real Z);
  
  //! For this XYZ object, assigns
  //! the values X, Y and Z to its three coordinates
    void SetCoord (const Standard_Real X, const Standard_Real Y, const Standard_Real Z);
  

  //! modifies the coordinate of range Index
  //! Index = 1 => X is modified
  //! Index = 2 => Y is modified
  //! Index = 3 => Z is modified
  //! Raises OutOfRange if Index != {1, 2, 3}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! Assigns the given value to the X coordinate
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the Y coordinate
    void SetY (const Standard_Real Y);
  
  //! Assigns the given value to the Z coordinate
    void SetZ (const Standard_Real Z);
  

  //! returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Index = 3 => Z is returned
  //!
  //! Raises OutOfRange if Index != {1, 2, 3}.
    Standard_Real Coord (const Standard_Integer Index) const;
  
    Standard_Real& ChangeCoord (const Standard_Integer theIndex);
  
    void Coord (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;

    //! Returns a const ptr to coordinates location.
    //! Is useful for algorithms, but DOES NOT PERFORM
    //! ANY CHECKS!
    inline const Standard_Real* GetData() const { return (&x); }

    //! Returns a ptr to coordinates location.
    //! Is useful for algorithms, but DOES NOT PERFORM
    //! ANY CHECKS!
    inline Standard_Real* ChangeData() { return (&x); }
  
  //! Returns the X coordinate
    Standard_Real X() const;
  
  //! Returns the Y coordinate
    Standard_Real Y() const;
  
  //! Returns the Z coordinate
    Standard_Real Z() const;
  
  //! computes Sqrt (X*X + Y*Y + Z*Z) where X, Y and Z are the three coordinates of this XYZ object.
    Standard_Real Modulus() const;
  
  //! Computes X*X + Y*Y + Z*Z where X, Y and Z are the three coordinates of this XYZ object.
    Standard_Real SquareModulus() const;
  

  //! Returns True if he coordinates of this XYZ object are
  //! equal to the respective coordinates Other,
  //! within the specified tolerance Tolerance. I.e.:
  //! abs(<me>.X() - Other.X()) <= Tolerance and
  //! abs(<me>.Y() - Other.Y()) <= Tolerance and
  //! abs(<me>.Z() - Other.Z()) <= Tolerance.
  Standard_EXPORT Standard_Boolean IsEqual (const gp_XYZ& Other, const Standard_Real Tolerance) const;
  

  //! <me>.X() = <me>.X() + Other.X()
  //! <me>.Y() = <me>.Y() + Other.Y()
  //! <me>.Z() = <me>.Z() + Other.Z()
    void Add (const gp_XYZ& Other);
  void operator += (const gp_XYZ& Other)
{
  Add(Other);
}
  

  //! new.X() = <me>.X() + Other.X()
  //! new.Y() = <me>.Y() + Other.Y()
  //! new.Z() = <me>.Z() + Other.Z()
    gp_XYZ Added (const gp_XYZ& Other) const;
  gp_XYZ operator + (const gp_XYZ& Other) const
{
  return Added(Other);
}
  

  //! <me>.X() = <me>.Y() * Other.Z() - <me>.Z() * Other.Y()
  //! <me>.Y() = <me>.Z() * Other.X() - <me>.X() * Other.Z()
  //! <me>.Z() = <me>.X() * Other.Y() - <me>.Y() * Other.X()
    void Cross (const gp_XYZ& Right);
  void operator ^= (const gp_XYZ& Right)
{
  Cross(Right);
}
  

  //! new.X() = <me>.Y() * Other.Z() - <me>.Z() * Other.Y()
  //! new.Y() = <me>.Z() * Other.X() - <me>.X() * Other.Z()
  //! new.Z() = <me>.X() * Other.Y() - <me>.Y() * Other.X()
    gp_XYZ Crossed (const gp_XYZ& Right) const;
  gp_XYZ operator ^ (const gp_XYZ& Right) const
{
  return Crossed(Right);
}
  

  //! Computes the magnitude of the cross product between <me> and
  //! Right. Returns || <me> ^ Right ||
    Standard_Real CrossMagnitude (const gp_XYZ& Right) const;
  

  //! Computes the square magnitude of the cross product between <me> and
  //! Right. Returns || <me> ^ Right ||**2
    Standard_Real CrossSquareMagnitude (const gp_XYZ& Right) const;
  
  //! Triple vector product
  //! Computes <me> = <me>.Cross(Coord1.Cross(Coord2))
    void CrossCross (const gp_XYZ& Coord1, const gp_XYZ& Coord2);
  
  //! Triple vector product
  //! computes New = <me>.Cross(Coord1.Cross(Coord2))
    gp_XYZ CrossCrossed (const gp_XYZ& Coord1, const gp_XYZ& Coord2) const;
  
  //! divides <me> by a real.
    void Divide (const Standard_Real Scalar);
  void operator /= (const Standard_Real Scalar)
{
  Divide(Scalar);
}
  
  //! divides <me> by a real.
    gp_XYZ Divided (const Standard_Real Scalar) const;
  gp_XYZ operator / (const Standard_Real Scalar) const
{
  return Divided(Scalar);
}
  
  //! computes the scalar product between <me> and Other
    Standard_Real Dot (const gp_XYZ& Other) const;
  Standard_Real operator * (const gp_XYZ& Other) const
{
  return Dot(Other);
}
  
  //! computes the triple scalar product
    Standard_Real DotCross (const gp_XYZ& Coord1, const gp_XYZ& Coord2) const;
  

  //! <me>.X() = <me>.X() * Scalar;
  //! <me>.Y() = <me>.Y() * Scalar;
  //! <me>.Z() = <me>.Z() * Scalar;
    void Multiply (const Standard_Real Scalar);
  void operator *= (const Standard_Real Scalar)
{
  Multiply(Scalar);
}
  

  //! <me>.X() = <me>.X() * Other.X();
  //! <me>.Y() = <me>.Y() * Other.Y();
  //! <me>.Z() = <me>.Z() * Other.Z();
    void Multiply (const gp_XYZ& Other);
  void operator *= (const gp_XYZ& Other)
{
  Multiply(Other);
}
  
  //! <me> = Matrix * <me>
    void Multiply (const gp_Mat& Matrix);
  void operator *= (const gp_Mat& Matrix)
{
  Multiply(Matrix);
}
  

  //! New.X() = <me>.X() * Scalar;
  //! New.Y() = <me>.Y() * Scalar;
  //! New.Z() = <me>.Z() * Scalar;
    gp_XYZ Multiplied (const Standard_Real Scalar) const;
  gp_XYZ operator * (const Standard_Real Scalar) const
{
  return Multiplied(Scalar);
}
  

  //! new.X() = <me>.X() * Other.X();
  //! new.Y() = <me>.Y() * Other.Y();
  //! new.Z() = <me>.Z() * Other.Z();
    gp_XYZ Multiplied (const gp_XYZ& Other) const;
  
  //! New = Matrix * <me>
    gp_XYZ Multiplied (const gp_Mat& Matrix) const;
  gp_XYZ operator * (const gp_Mat& Matrix) const
{
  return Multiplied(Matrix);
}
  

  //! <me>.X() = <me>.X()/ <me>.Modulus()
  //! <me>.Y() = <me>.Y()/ <me>.Modulus()
  //! <me>.Z() = <me>.Z()/ <me>.Modulus()
  //! Raised if <me>.Modulus() <= Resolution from gp
    void Normalize();
  

  //! New.X() = <me>.X()/ <me>.Modulus()
  //! New.Y() = <me>.Y()/ <me>.Modulus()
  //! New.Z() = <me>.Z()/ <me>.Modulus()
  //! Raised if <me>.Modulus() <= Resolution from gp
    gp_XYZ Normalized() const;
  

  //! <me>.X() = -<me>.X()
  //! <me>.Y() = -<me>.Y()
  //! <me>.Z() = -<me>.Z()
    void Reverse();
  

  //! New.X() = -<me>.X()
  //! New.Y() = -<me>.Y()
  //! New.Z() = -<me>.Z()
    gp_XYZ Reversed() const;
  

  //! <me>.X() = <me>.X() - Other.X()
  //! <me>.Y() = <me>.Y() - Other.Y()
  //! <me>.Z() = <me>.Z() - Other.Z()
    void Subtract (const gp_XYZ& Right);
  void operator -= (const gp_XYZ& Right)
{
  Subtract(Right);
}
  

  //! new.X() = <me>.X() - Other.X()
  //! new.Y() = <me>.Y() - Other.Y()
  //! new.Z() = <me>.Z() - Other.Z()
    gp_XYZ Subtracted (const gp_XYZ& Right) const;
  gp_XYZ operator - (const gp_XYZ& Right) const
{
  return Subtracted(Right);
}
  

  //! <me> is set to the following linear form :
  //! A1 * XYZ1 + A2 * XYZ2 + A3 * XYZ3 + XYZ4
    void SetLinearForm (const Standard_Real A1, const gp_XYZ& XYZ1, const Standard_Real A2, const gp_XYZ& XYZ2, const Standard_Real A3, const gp_XYZ& XYZ3, const gp_XYZ& XYZ4);
  

  //! <me> is set to the following linear form :
  //! A1 * XYZ1 + A2 * XYZ2 + A3 * XYZ3
    void SetLinearForm (const Standard_Real A1, const gp_XYZ& XYZ1, const Standard_Real A2, const gp_XYZ& XYZ2, const Standard_Real A3, const gp_XYZ& XYZ3);
  

  //! <me> is set to the following linear form :
  //! A1 * XYZ1 + A2 * XYZ2 + XYZ3
    void SetLinearForm (const Standard_Real A1, const gp_XYZ& XYZ1, const Standard_Real A2, const gp_XYZ& XYZ2, const gp_XYZ& XYZ3);
  

  //! <me> is set to the following linear form :
  //! A1 * XYZ1 + A2 * XYZ2
    void SetLinearForm (const Standard_Real A1, const gp_XYZ& XYZ1, const Standard_Real A2, const gp_XYZ& XYZ2);
  

  //! <me> is set to the following linear form :
  //! A1 * XYZ1 + XYZ2
    void SetLinearForm (const Standard_Real A1, const gp_XYZ& XYZ1, const gp_XYZ& XYZ2);
  

  //! <me> is set to the following linear form :
  //! XYZ1 + XYZ2
    void SetLinearForm (const gp_XYZ& XYZ1, const gp_XYZ& XYZ2);




protected:





private:



  Standard_Real x;
  Standard_Real y;
  Standard_Real z;


};


#include <gp_XYZ.lxx>





#endif // _gp_XYZ_HeaderFile
