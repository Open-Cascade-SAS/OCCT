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

#ifndef _gp_Trsf2d_HeaderFile
#define _gp_Trsf2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <gp_TrsfForm.hxx>
#include <gp_Mat2d.hxx>
#include <gp_XY.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_GTrsf2d;
class gp_Trsf;
class gp_Pnt2d;
class gp_Ax2d;
class gp_Vec2d;
class gp_XY;
class gp_Mat2d;



//! Defines a non-persistent transformation in 2D space.
//! The following transformations are implemented :
//! . Translation, Rotation, Scale
//! . Symmetry with respect to a point and a line.
//! Complex transformations can be obtained by combining the
//! previous elementary transformations using the method Multiply.
//! The transformations can be represented as follow :
//!
//! V1   V2   T       XY        XY
//! | a11  a12  a13 |   | x |     | x'|
//! | a21  a22  a23 |   | y |     | y'|
//! |  0    0    1  |   | 1 |     | 1 |
//!
//! where {V1, V2} defines the vectorial part of the transformation
//! and T defines the translation part of the transformation.
//! This transformation never change the nature of the objects.
class gp_Trsf2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns identity transformation.
    gp_Trsf2d();
  
  //! Creates a 2d transformation in the XY plane from a
  //! 3d transformation .
    gp_Trsf2d(const gp_Trsf& T);
  

  //! Changes the transformation into a symmetrical transformation.
  //! P is the center of the symmetry.
    void SetMirror (const gp_Pnt2d& P);
  

  //! Changes the transformation into a symmetrical transformation.
  //! A is the center of the axial symmetry.
  Standard_EXPORT void SetMirror (const gp_Ax2d& A);
  

  //! Changes the transformation into a rotation.
  //! P is the rotation's center and Ang is the angular value of the
  //! rotation in radian.
    void SetRotation (const gp_Pnt2d& P, const Standard_Real Ang);
  

  //! Changes the transformation into a scale.
  //! P is the center of the scale and S is the scaling value.
    void SetScale (const gp_Pnt2d& P, const Standard_Real S);
  

  //! Changes a transformation allowing passage from the coordinate
  //! system "FromSystem1" to the coordinate system "ToSystem2".
  Standard_EXPORT void SetTransformation (const gp_Ax2d& FromSystem1, const gp_Ax2d& ToSystem2);
  

  //! Changes the transformation allowing passage from the basic
  //! coordinate system
  //! {P(0.,0.,0.), VX (1.,0.,0.), VY (0.,1.,0.)}
  //! to the local coordinate system defined with the Ax2d ToSystem.
  Standard_EXPORT void SetTransformation (const gp_Ax2d& ToSystem);
  

  //! Changes the transformation into a translation.
  //! V is the vector of the translation.
    void SetTranslation (const gp_Vec2d& V);
  

  //! Makes the transformation into a translation from
  //! the point P1 to the point P2.
    void SetTranslation (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
  //! Replaces the translation vector with V.
  Standard_EXPORT void SetTranslationPart (const gp_Vec2d& V);
  
  //! Modifies the scale factor.
  Standard_EXPORT void SetScaleFactor (const Standard_Real S);
  
  //! Returns true if the determinant of the vectorial part of
  //! this transformation is negative..
    Standard_Boolean IsNegative() const;
  

  //! Returns the nature of the transformation. It can be  an
  //! identity transformation, a rotation, a translation, a mirror
  //! (relative to a point or an axis), a scaling transformation,
  //! or a compound transformation.
    gp_TrsfForm Form() const;
  
  //! Returns the scale factor.
    Standard_Real ScaleFactor() const;
  

  //! Returns the translation part of the transformation's matrix
    const gp_XY& TranslationPart() const;
  

  //! Returns the vectorial part of the transformation. It is a
  //! 2*2 matrix which includes the scale factor.
  Standard_EXPORT gp_Mat2d VectorialPart() const;
  

  //! Returns the homogeneous vectorial part of the transformation.
  //! It is a 2*2 matrix which doesn't include the scale factor.
  //! The coefficients of this matrix must be multiplied by the
  //! scale factor to obtain the coefficients of the transformation.
    const gp_Mat2d& HVectorialPart() const;
  

  //! Returns the angle corresponding to the rotational component
  //! of the transformation matrix (operation opposite to SetRotation()).
  Standard_EXPORT Standard_Real RotationPart() const;
  

  //! Returns the coefficients of the transformation's matrix.
  //! It is a 2 rows * 3 columns matrix.
  //! Raises OutOfRange if Row < 1 or Row > 2 or Col < 1 or Col > 3
    Standard_Real Value (const Standard_Integer Row, const Standard_Integer Col) const;
  
  Standard_EXPORT void Invert();
  

  //! Computes the reverse transformation.
  //! Raises an exception if the matrix of the transformation
  //! is not inversible, it means that the scale factor is lower
  //! or equal to Resolution from package gp.
    gp_Trsf2d Inverted() const;
  
    gp_Trsf2d Multiplied (const gp_Trsf2d& T) const;
  gp_Trsf2d operator * (const gp_Trsf2d& T) const
{
  return Multiplied(T);
}
  

  //! Computes the transformation composed from <me> and T.
  //! <me> = <me> * T
  Standard_EXPORT void Multiply (const gp_Trsf2d& T);
void operator *= (const gp_Trsf2d& T)
{
  Multiply(T);
}
  

  //! Computes the transformation composed from <me> and T.
  //! <me> = T * <me>
  Standard_EXPORT void PreMultiply (const gp_Trsf2d& T);
  
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Computes the following composition of transformations
  //! <me> * <me> * .......* <me>,  N time.
  //! if N = 0 <me> = Identity
  //! if N < 0 <me> = <me>.Inverse() *...........* <me>.Inverse().
  //!
  //! Raises if N < 0 and if the matrix of the transformation not
  //! inversible.
    gp_Trsf2d Powered (const Standard_Integer N);
  
    void Transforms (Standard_Real& X, Standard_Real& Y) const;
  
  //! Transforms  a doublet XY with a Trsf2d
    void Transforms (gp_XY& Coord) const;
  
  //! Sets the coefficients  of the transformation. The
  //! transformation  of the  point  x,y is  the point
  //! x',y' with :
  //!
  //! x' = a11 x + a12 y + a13
  //! y' = a21 x + a22 y + a23
  //!
  //! The method Value(i,j) will return aij.
  //! Raises ConstructionError if the determinant of the aij is null.
  //! If the matrix as not a uniform scale it will be orthogonalized before future using.
  Standard_EXPORT void SetValues (const Standard_Real a11, const Standard_Real a12, const Standard_Real a13, const Standard_Real a21, const Standard_Real a22, const Standard_Real a23);


friend class gp_GTrsf2d;


protected:

  
  //! Makes orthogonalization of "matrix"
  Standard_EXPORT void Orthogonalize();




private:



  Standard_Real scale;
  gp_TrsfForm shape;
  gp_Mat2d matrix;
  gp_XY loc;


};


#include <gp_Trsf2d.lxx>





#endif // _gp_Trsf2d_HeaderFile
