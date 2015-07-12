// Created on: 1993-03-10
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

#ifndef _Geom_Transformation_HeaderFile
#define _Geom_Transformation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Trsf.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_TrsfForm.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Trsf;
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Ax3;
class gp_Vec;


class Geom_Transformation;
DEFINE_STANDARD_HANDLE(Geom_Transformation, MMgt_TShared)

//! Describes how to construct the following elementary transformations
//! - translations,
//! - rotations,
//! - symmetries,
//! - scales.
//! The Transformation class can also be used to
//! construct complex transformations by combining these
//! elementary transformations.
//! However, these transformations can never change
//! the type of an object. For example, the projection
//! transformation can change a circle into an ellipse, and
//! therefore change the real type of the object. Such a
//! transformation is forbidden in this environment and
//! cannot be a Geom_Transformation.
//! The transformation can be represented as follow :
//!
//! V1   V2   V3    T
//! | a11  a12  a13   a14 |   | x |      | x'|
//! | a21  a22  a23   a24 |   | y |      | y'|
//! | a31  a32  a33   a34 |   | z |   =  | z'|
//! |  0    0    0     1  |   | 1 |      | 1 |
//!
//! where {V1, V2, V3} defines the vectorial part of the
//! transformation and T defines the translation part of
//! the transformation.
//! Note: Geom_Transformation transformations
//! provide the same kind of "geometric" services as
//! gp_Trsf ones but have more complex data structures.
//! The geometric objects provided by the Geom
//! package use gp_Trsf transformations in the syntaxes
//! Transform and Transformed.
//! Geom_Transformation transformations are used in
//! a context where they can be shared by several
//! objects contained inside a common data structure.
class Geom_Transformation : public MMgt_TShared
{

public:

  
  //! Creates an identity transformation.
  Standard_EXPORT Geom_Transformation();
  
  //! Creates a transient copy of T.
  Standard_EXPORT Geom_Transformation(const gp_Trsf& T);
  

  //! Makes the transformation into a symmetrical transformation
  //! with respect to a point P.
  //! P is the center of the symmetry.
  Standard_EXPORT void SetMirror (const gp_Pnt& P);
  

  //! Makes the transformation into a symmetrical transformation
  //! with respect to an axis A1.
  //! A1 is the center of the axial symmetry.
  Standard_EXPORT void SetMirror (const gp_Ax1& A1);
  

  //! Makes the transformation into a symmetrical transformation
  //! with respect to a plane.  The plane of the symmetry is
  //! defined with the axis placement A2. It is the plane
  //! (Location, XDirection, YDirection).
  Standard_EXPORT void SetMirror (const gp_Ax2& A2);
  

  //! Makes the transformation into a rotation.
  //! A1 is the axis rotation and Ang is the angular value
  //! of the rotation in radians.
  Standard_EXPORT void SetRotation (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Makes the transformation into a scale. P is the center of
  //! the scale and S is the scaling value.
  Standard_EXPORT void SetScale (const gp_Pnt& P, const Standard_Real S);
  

  //! Makes a transformation allowing passage from the coordinate
  //! system "FromSystem1" to the coordinate system "ToSystem2".
  //! Example :
  //! In a C++ implementation :
  //! Real x1, y1, z1;  // are the coordinates of a point in the
  //! // local system FromSystem1
  //! Real x2, y2, z2;  // are the coordinates of a point in the
  //! // local system ToSystem2
  //! gp_Pnt P1 (x1, y1, z1)
  //! Geom_Transformation T;
  //! T.SetTransformation (FromSystem1, ToSystem2);
  //! gp_Pnt P2 = P1.Transformed (T);
  //! P2.Coord (x2, y2, z2);
  Standard_EXPORT void SetTransformation (const gp_Ax3& FromSystem1, const gp_Ax3& ToSystem2);
  

  //! Makes the transformation allowing passage from the basic
  //! coordinate system
  //! {P(0.,0.,0.), VX (1.,0.,0.), VY (0.,1.,0.), VZ (0., 0. ,1.) }
  //! to the local coordinate system defined with the Ax2 ToSystem.
  //! Same utilisation as the previous method. FromSystem1 is
  //! defaulted to the absolute coordinate system.
  Standard_EXPORT void SetTransformation (const gp_Ax3& ToSystem);
  

  //! Makes the transformation into a translation.
  //! V is the vector of the translation.
  Standard_EXPORT void SetTranslation (const gp_Vec& V);
  

  //! Makes the transformation into a translation from the point
  //! P1 to the point P2.
  Standard_EXPORT void SetTranslation (const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Converts the gp_Trsf transformation T into this transformation.
  Standard_EXPORT void SetTrsf (const gp_Trsf& T);
  
  //! Checks whether this transformation is an indirect
  //! transformation: returns true if the determinant of the
  //! matrix of the vectorial part of the transformation is less than 0.
  Standard_EXPORT Standard_Boolean IsNegative() const;
  
  //! Returns the nature of this transformation as a value
  //! of the gp_TrsfForm enumeration.
  Standard_EXPORT gp_TrsfForm Form() const;
  
  //! Returns the scale value of the transformation.
  Standard_EXPORT Standard_Real ScaleFactor() const;
  

  //! Returns a non transient copy of <me>.
  Standard_EXPORT const gp_Trsf& Trsf() const;
  

  //! Returns the coefficients of the global matrix of tranformation.
  //! It is a 3 rows X 4 columns matrix.
  //!
  //! Raised if  Row < 1 or Row > 3  or  Col < 1 or Col > 4
  //!
  //! Computes the reverse transformation.
  Standard_EXPORT Standard_Real Value (const Standard_Integer Row, const Standard_Integer Col) const;
  

  //! Raised if the the transformation is singular. This means that
  //! the ScaleFactor is lower or equal to Resolution from
  //! package gp.
  Standard_EXPORT void Invert();
  

  //! Raised if the the transformation is singular. This means that
  //! the ScaleFactor is lower or equal to Resolution from
  //! package gp.
  Standard_EXPORT Handle(Geom_Transformation) Inverted() const;
  

  //! Computes the transformation composed with Other and <me>.
  //! <me> * Other.
  //! Returns a new transformation
  Standard_EXPORT Handle(Geom_Transformation) Multiplied (const Handle(Geom_Transformation)& Other) const;
  

  //! Computes the transformation composed with Other and <me> .
  //! <me> = <me> * Other.
  Standard_EXPORT void Multiply (const Handle(Geom_Transformation)& Other);
  

  //! Computes the following composition of transformations
  //! if N > 0  <me> * <me> * .......* <me>.
  //! if N = 0  Identity
  //! if N < 0  <me>.Invert() * .........* <me>.Invert()
  //!
  //! Raised if N < 0 and if the transformation is not inversible
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Raised if N < 0 and if the transformation is not inversible
  Standard_EXPORT Handle(Geom_Transformation) Powered (const Standard_Integer N) const;
  

  //! Computes the matrix of the transformation composed with
  //! <me> and Other.     <me> = Other * <me>
  Standard_EXPORT void PreMultiply (const Handle(Geom_Transformation)& Other);
  

  //! Applies the transformation <me> to the triplet {X, Y, Z}.
  Standard_EXPORT void Transforms (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;
  
  //! Creates a new object which is a copy of this transformation.
  Standard_EXPORT Handle(Geom_Transformation) Copy() const;




  DEFINE_STANDARD_RTTI(Geom_Transformation,MMgt_TShared)

protected:




private:


  gp_Trsf gpTrsf;


};







#endif // _Geom_Transformation_HeaderFile
