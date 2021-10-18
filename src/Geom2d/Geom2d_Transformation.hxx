// Created on: 1993-03-24
// Created by: JCV
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

#ifndef _Geom2d_Transformation_HeaderFile
#define _Geom2d_Transformation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Trsf2d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_TrsfForm.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;
class gp_Ax2d;
class gp_Vec2d;


class Geom2d_Transformation;
DEFINE_STANDARD_HANDLE(Geom2d_Transformation, Standard_Transient)


//! The class Transformation allows to create Translation,
//! Rotation, Symmetry, Scaling and complex transformations
//! obtained by combination of the previous elementary
//! transformations.
//! The Transformation class can also be used to
//! construct complex transformations by combining
//! these elementary transformations.
//! However, these transformations can never change
//! the type of an object. For example, the projection
//! transformation can change a circle into an ellipse,
//! and therefore change the real type of the object.
//! Such a transformation is forbidden in this
//! environment and cannot be a Geom2d_Transformation.
//! The transformation can be represented as follow :
//!
//! V1   V2     T
//! | a11  a12    a14 |   | x |      | x'|
//! | a21  a22    a24 |   | y |      | y'|
//! |  0    0      1  |   | 1 |      | 1 |
//!
//! where {V1, V2} defines the vectorial part of the
//! transformation and T defines the translation part of
//! the transformation.
//! - Geom2d_Transformation transformations provide
//! the same kind of "geometric" services as
//! gp_Trsf2d ones but have more complex data
//! structures. The geometric objects provided by the
//! Geom2d package use gp_Trsf2d transformations
//! in the syntaxes Transform and Transformed.
//! - Geom2d_Transformation transformations are
//! used in a context where they can be shared by
//! several objects contained inside a common data structure.
class Geom2d_Transformation : public Standard_Transient
{

public:

  
  //! Creates an identity transformation.
  Standard_EXPORT Geom2d_Transformation();
  
  //! Creates a persistent copy of T.
  Standard_EXPORT Geom2d_Transformation(const gp_Trsf2d& T);
  

  //! Makes the transformation into a symmetrical transformation
  //! with respect to a point P.
  //! P is the center of the symmetry.
  Standard_EXPORT void SetMirror (const gp_Pnt2d& P);
  

  //! Makes the transformation into a symmetrical transformation
  //! with respect to an axis A.
  //! A is the center of the axial symmetry.
  Standard_EXPORT void SetMirror (const gp_Ax2d& A);
  
  //! Assigns to this transformation the geometric
  //! properties of a rotation at angle Ang (in radians) about point P.
  Standard_EXPORT void SetRotation (const gp_Pnt2d& P, const Standard_Real Ang);
  

  //! Makes the transformation into a scale. P is the center of
  //! the scale and S is the scaling value.
  Standard_EXPORT void SetScale (const gp_Pnt2d& P, const Standard_Real S);
  

  //! Makes a transformation allowing passage from the coordinate
  //! system "FromSystem1" to the coordinate system "ToSystem2".
  Standard_EXPORT void SetTransformation (const gp_Ax2d& FromSystem1, const gp_Ax2d& ToSystem2);
  

  //! Makes the transformation allowing passage from the basic
  //! coordinate system
  //! {P(0.,0.,0.), VX (1.,0.,0.), VY (0.,1.,0.)}
  //! to the local coordinate system defined with the Ax2d ToSystem.
  Standard_EXPORT void SetTransformation (const gp_Ax2d& ToSystem);
  

  //! Makes the transformation into a translation.
  //! V is the vector of the translation.
  Standard_EXPORT void SetTranslation (const gp_Vec2d& V);
  

  //! Makes the transformation into a translation from the point
  //! P1 to the point P2.
  Standard_EXPORT void SetTranslation (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  

  //! Makes the transformation into a transformation T from
  //! package gp.
  Standard_EXPORT void SetTrsf2d (const gp_Trsf2d& T);
  
  //! Checks whether this transformation is an indirect
  //! transformation: returns true if the determinant of the
  //! matrix of the vectorial part of the transformation is less than 0.
  Standard_EXPORT Standard_Boolean IsNegative() const;
  
  //! Returns the nature of this transformation as a value
  //! of the gp_TrsfForm enumeration.
  //! Returns the nature of the transformation. It can be
  //! Identity, Rotation, Translation, PntMirror, Ax1Mirror,
  //! Scale, CompoundTrsf
  Standard_EXPORT gp_TrsfForm Form() const;
  
  //! Returns the scale value of the transformation.
  Standard_EXPORT Standard_Real ScaleFactor() const;
  
  //! Converts this transformation into a gp_Trsf2d transformation.
  //! Returns a non persistent copy of <me>.
  //! -C++: return const&
  Standard_EXPORT gp_Trsf2d Trsf2d() const;
  

  //! Returns the coefficients of the global matrix of transformation.
  //! It is a 2 rows X 3 columns matrix.
  //!
  //! Raised if  Row < 1 or Row > 2  or  Col < 1 or Col > 2
  //!
  //! Computes the reverse transformation.
  Standard_EXPORT Standard_Real Value (const Standard_Integer Row, const Standard_Integer Col) const;
  
  //! Computes the inverse of this transformation.
  //! and  assigns the result to this transformatio
  //!
  //! Raised if the transformation is singular. This means that
  //! the ScaleFactor is lower or equal to Resolution from
  //! package gp.
  Standard_EXPORT void Invert();
  
  //! Computes the inverse of this transformation and creates a new one.
  //! Raises ConstructionError  if the transformation is singular. This means that
  //! the ScaleFactor is lower or equal to Resolution from package gp.
  Standard_NODISCARD Standard_EXPORT Handle(Geom2d_Transformation) Inverted() const;
  

  //! Computes the transformation composed with Other and <me>.
  //! <me> * Other.
  //! Returns a new transformation
  Standard_NODISCARD Standard_EXPORT
	Handle(Geom2d_Transformation) Multiplied (const Handle(Geom2d_Transformation)& Other) const;
Standard_NODISCARD Handle(Geom2d_Transformation) operator * (const Handle(Geom2d_Transformation)& Other) const
{
  return Multiplied(Other);
}
  

  //! Computes the transformation composed with Other and <me> .
  //! <me> = <me> * Other.
  //!
  //! Computes the following composition of transformations
  //! if N > 0  <me> * <me> * .......* <me>.
  //! if N = 0  Identity
  //! if N < 0  <me>.Invert() * .........* <me>.Invert()
  Standard_EXPORT void Multiply (const Handle(Geom2d_Transformation)& Other);
void operator *= (const Handle(Geom2d_Transformation)& Other)
{
  Multiply(Other);
}
  

  //! Raised if N < 0 and if the transformation is not inversible
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Raised if N < 0 and if the transformation is not inversible
  Standard_EXPORT Handle(Geom2d_Transformation) Powered (const Standard_Integer N) const;
  

  //! Computes the matrix of the transformation composed with
  //! <me> and Other.     <me> = Other * <me>
  Standard_EXPORT void PreMultiply (const Handle(Geom2d_Transformation)& Other);
  

  //! Applies the transformation <me> to the triplet {X, Y}.
  Standard_EXPORT void Transforms (Standard_Real& X, Standard_Real& Y) const;
  
  //! Creates a new object, which is a copy of this transformation.
  Standard_EXPORT Handle(Geom2d_Transformation) Copy() const;




  DEFINE_STANDARD_RTTIEXT(Geom2d_Transformation,Standard_Transient)

protected:




private:


  gp_Trsf2d gpTrsf2d;


};







#endif // _Geom2d_Transformation_HeaderFile
