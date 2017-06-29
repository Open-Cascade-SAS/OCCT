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

#ifndef _gp_Trsf_HeaderFile
#define _gp_Trsf_HeaderFile

#include <gp_TrsfForm.hxx>
#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Mat4.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>

class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Quaternion;
class gp_Ax3;
class gp_Vec;

// Avoid possible conflict with SetForm macro defined by windows.h
#ifdef SetForm
#undef SetForm
#endif

//! Defines a non-persistent transformation in 3D space.
//! The following transformations are implemented :
//! . Translation, Rotation, Scale
//! . Symmetry with respect to a point, a line, a plane.
//! Complex transformations can be obtained by combining the
//! previous elementary transformations using the method
//! Multiply.
//! The transformations can be represented as follow :
//!
//! V1   V2   V3    T       XYZ        XYZ
//! | a11  a12  a13   a14 |   | x |      | x'|
//! | a21  a22  a23   a24 |   | y |      | y'|
//! | a31  a32  a33   a34 |   | z |   =  | z'|
//! |  0    0    0     1  |   | 1 |      | 1 |
//!
//! where {V1, V2, V3} defines the vectorial part of the
//! transformation and T defines the translation part of the
//! transformation.
//! This transformation never change the nature of the objects.
class gp_Trsf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the identity transformation.
    gp_Trsf();
  
  //! Creates  a 3D transformation from the 2D transformation T.
  //! The resulting transformation has a homogeneous
  //! vectorial part, V3, and a translation part, T3, built from T:
  //! a11    a12
  //! 0             a13
  //! V3 =    a21    a22    0       T3
  //! =   a23
  //! 0    0    1.
  //! 0
  //! It also has the same scale factor as T. This
  //! guarantees (by projection) that the transformation
  //! which would be performed by T in a plane (2D space)
  //! is performed by the resulting transformation in the xOy
  //! plane of the 3D space, (i.e. in the plane defined by the
  //! origin (0., 0., 0.) and the vectors DX (1., 0., 0.), and DY
  //! (0., 1., 0.)). The scale factor is applied to the entire space.
  Standard_EXPORT gp_Trsf(const gp_Trsf2d& T);
  

  //! Makes the transformation into a symmetrical transformation.
  //! P is the center of the symmetry.
    void SetMirror (const gp_Pnt& P);
  

  //! Makes the transformation into a symmetrical transformation.
  //! A1 is the center of the axial symmetry.
  Standard_EXPORT void SetMirror (const gp_Ax1& A1);
  

  //! Makes the transformation into a symmetrical transformation.
  //! A2 is the center of the planar symmetry
  //! and defines the plane of symmetry by its origin, "X
  //! Direction" and "Y Direction".
  Standard_EXPORT void SetMirror (const gp_Ax2& A2);
  

  //! Changes the transformation into a rotation.
  //! A1 is the rotation axis and Ang is the angular value of the
  //! rotation in radians.
  Standard_EXPORT void SetRotation (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Changes the transformation into a rotation defined by quaternion.
  //! Note that rotation is performed around origin, i.e.
  //! no translation is involved.
  Standard_EXPORT void SetRotation (const gp_Quaternion& R);
  

  //! Changes the transformation into a scale.
  //! P is the center of the scale and S is the scaling value.
  //! Raises ConstructionError  If <S> is null.
  Standard_EXPORT void SetScale (const gp_Pnt& P, const Standard_Real S);
  

  //! Modifies this transformation so that it transforms the
  //! coordinate system defined by FromSystem1 into the
  //! one defined by ToSystem2. After this modification, this
  //! transformation transforms:
  //! -   the origin of FromSystem1 into the origin of ToSystem2,
  //! -   the "X Direction" of FromSystem1 into the "X
  //! Direction" of ToSystem2,
  //! -   the "Y Direction" of FromSystem1 into the "Y
  //! Direction" of ToSystem2, and
  //! -   the "main Direction" of FromSystem1 into the "main
  //! Direction" of ToSystem2.
  //! Warning
  //! When you know the coordinates of a point in one
  //! coordinate system and you want to express these
  //! coordinates in another one, do not use the
  //! transformation resulting from this function. Use the
  //! transformation that results from SetTransformation instead.
  //! SetDisplacement and SetTransformation create
  //! related transformations: the vectorial part of one is the
  //! inverse of the vectorial part of the other.
  Standard_EXPORT void SetDisplacement (const gp_Ax3& FromSystem1, const gp_Ax3& ToSystem2);
  
  //! Modifies this transformation so that it transforms the
  //! coordinates of any point, (x, y, z), relative to a source
  //! coordinate system into the coordinates (x', y', z') which
  //! are relative to a target coordinate system, but which
  //! represent the same point
  //! The transformation is from the coordinate
  //! system "FromSystem1" to the coordinate system "ToSystem2".
  //! Example :
  //! In a C++ implementation :
  //! Real x1, y1, z1;  // are the coordinates of a point in the
  //! // local system FromSystem1
  //! Real x2, y2, z2;  // are the coordinates of a point in the
  //! // local system ToSystem2
  //! gp_Pnt P1 (x1, y1, z1)
  //! Trsf T;
  //! T.SetTransformation (FromSystem1, ToSystem2);
  //! gp_Pnt P2 = P1.Transformed (T);
  //! P2.Coord (x2, y2, z2);
  Standard_EXPORT void SetTransformation (const gp_Ax3& FromSystem1, const gp_Ax3& ToSystem2);
  
  //! Modifies this transformation so that it transforms the
  //! coordinates of any point, (x, y, z), relative to a source
  //! coordinate system into the coordinates (x', y', z') which
  //! are relative to a target coordinate system, but which
  //! represent the same point
  //! The transformation is from the default coordinate system
  //! {P(0.,0.,0.), VX (1.,0.,0.), VY (0.,1.,0.), VZ (0., 0. ,1.) }
  //! to the local coordinate system defined with the Ax3 ToSystem.
  //! Use in the same way  as the previous method. FromSystem1 is
  //! defaulted to the absolute coordinate system.
  Standard_EXPORT void SetTransformation (const gp_Ax3& ToSystem);
  

  //! Sets transformation by directly specified rotation and translation.
  Standard_EXPORT void SetTransformation (const gp_Quaternion& R, const gp_Vec& T);
  

  //! Changes the transformation into a translation.
  //! V is the vector of the translation.
    void SetTranslation (const gp_Vec& V);
  

  //! Makes the transformation into a translation where the translation vector
  //! is the vector (P1, P2) defined from point P1 to point P2.
    void SetTranslation (const gp_Pnt& P1, const gp_Pnt& P2);
  
  //! Replaces the translation vector with the vector V.
  Standard_EXPORT void SetTranslationPart (const gp_Vec& V);
  
  //! Modifies the scale factor.
  //! Raises ConstructionError  If S is null.
  Standard_EXPORT void SetScaleFactor (const Standard_Real S);
  
  void SetForm (const gp_TrsfForm P);
  
  //! Sets the coefficients  of the transformation.  The
  //! transformation  of the  point  x,y,z is  the point
  //! x',y',z' with :
  //!
  //! x' = a11 x + a12 y + a13 z + a14
  //! y' = a21 x + a22 y + a23 z + a24
  //! z' = a31 x + a32 y + a33 z + a34
  //!
  //! The method Value(i,j) will return aij.
  //! Raises ConstructionError if the determinant of  the aij is null.
  //! The matrix is orthogonalized before future using.
  Standard_EXPORT void SetValues (const Standard_Real a11, const Standard_Real a12, const Standard_Real a13, const Standard_Real a14, const Standard_Real a21, const Standard_Real a22, const Standard_Real a23, const Standard_Real a24, const Standard_Real a31, const Standard_Real a32, const Standard_Real a33, const Standard_Real a34);
  
  //! Returns true if the determinant of the vectorial part of
  //! this transformation is negative.
    Standard_Boolean IsNegative() const;
  

  //! Returns the nature of the transformation. It can be: an
  //! identity transformation, a rotation, a translation, a mirror
  //! transformation (relative to a point, an axis or a plane), a
  //! scaling transformation, or a compound transformation.
    gp_TrsfForm Form() const;
  
  //! Returns the scale factor.
    Standard_Real ScaleFactor() const;
  

  //! Returns the translation part of the transformation's matrix
    const gp_XYZ& TranslationPart() const;
  

  //! Returns the boolean True if there is non-zero rotation.
  //! In the presence of rotation, the output parameters store the axis
  //! and the angle of rotation. The method always returns positive
  //! value "theAngle", i.e., 0. < theAngle <= PI.
  //! Note that this rotation is defined only by the vectorial part of
  //! the transformation; generally you would need to check also the
  //! translational part to obtain the axis (gp_Ax1) of rotation.
  Standard_EXPORT Standard_Boolean GetRotation (gp_XYZ& theAxis, Standard_Real& theAngle) const;
  

  //! Returns quaternion representing rotational part of the transformation.
  Standard_EXPORT gp_Quaternion GetRotation() const;
  

  //! Returns the vectorial part of the transformation. It is
  //! a 3*3 matrix which includes the scale factor.
  Standard_EXPORT gp_Mat VectorialPart() const;
  

  //! Computes the homogeneous vectorial part of the transformation.
  //! It is a 3*3 matrix which doesn't include the scale factor.
  //! In other words, the vectorial part of this transformation is equal
  //! to its homogeneous vectorial part, multiplied by the scale factor.
  //! The coefficients of this matrix must be multiplied by the
  //! scale factor to obtain the coefficients of the transformation.
    const gp_Mat& HVectorialPart() const;
  

  //! Returns the coefficients of the transformation's matrix.
  //! It is a 3 rows * 4 columns matrix.
  //! This coefficient includes the scale factor.
  //! Raises OutOfRanged if Row < 1 or Row > 3 or Col < 1 or Col > 4
    Standard_Real Value (const Standard_Integer Row, const Standard_Integer Col) const;
  
  Standard_EXPORT void Invert();
  

  //! Computes the reverse transformation
  //! Raises an exception if the matrix of the transformation
  //! is not inversible, it means that the scale factor is lower
  //! or equal to Resolution from package gp.
  //! Computes the transformation composed with T and  <me>.
  //! In a C++ implementation you can also write Tcomposed = <me> * T.
  //! Example :
  //! Trsf T1, T2, Tcomp; ...............
  //! Tcomp = T2.Multiplied(T1);         // or   (Tcomp = T2 * T1)
  //! Pnt P1(10.,3.,4.);
  //! Pnt P2 = P1.Transformed(Tcomp);    //using Tcomp
  //! Pnt P3 = P1.Transformed(T1);       //using T1 then T2
  //! P3.Transform(T2);                  // P3 = P2 !!!
    gp_Trsf Inverted() const;
  
    gp_Trsf Multiplied (const gp_Trsf& T) const;
  gp_Trsf operator * (const gp_Trsf& T) const
{
  return Multiplied(T);
}
  

  //! Computes the transformation composed with <me> and T.
  //! <me> = <me> * T
  Standard_EXPORT void Multiply (const gp_Trsf& T);
void operator *= (const gp_Trsf& T)
{
  Multiply(T);
}
  

  //! Computes the transformation composed with <me> and T.
  //! <me> = T * <me>
  Standard_EXPORT void PreMultiply (const gp_Trsf& T);
  
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Computes the following composition of transformations
  //! <me> * <me> * .......* <me>, N time.
  //! if N = 0 <me> = Identity
  //! if N < 0 <me> = <me>.Inverse() *...........* <me>.Inverse().
  //!
  //! Raises if N < 0 and if the matrix of the transformation not
  //! inversible.
    gp_Trsf Powered (const Standard_Integer N) const;
  
    void Transforms (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;
  
  //! Transformation of a triplet XYZ with a Trsf
    void Transforms (gp_XYZ& Coord) const;

  //! Convert transformation to 4x4 matrix.
  template<class T>
  void GetMat4 (NCollection_Mat4<T>& theMat) const
  {
    if (shape == gp_Identity)
    {
      theMat.InitIdentity();
      return;
    }

    theMat.SetValue (0, 0, static_cast<T> (Value (1, 1)));
    theMat.SetValue (0, 1, static_cast<T> (Value (1, 2)));
    theMat.SetValue (0, 2, static_cast<T> (Value (1, 3)));
    theMat.SetValue (0, 3, static_cast<T> (Value (1, 4)));
    theMat.SetValue (1, 0, static_cast<T> (Value (2, 1)));
    theMat.SetValue (1, 1, static_cast<T> (Value (2, 2)));
    theMat.SetValue (1, 2, static_cast<T> (Value (2, 3)));
    theMat.SetValue (1, 3, static_cast<T> (Value (2, 4)));
    theMat.SetValue (2, 0, static_cast<T> (Value (3, 1)));
    theMat.SetValue (2, 1, static_cast<T> (Value (3, 2)));
    theMat.SetValue (2, 2, static_cast<T> (Value (3, 3)));
    theMat.SetValue (2, 3, static_cast<T> (Value (3, 4)));
    theMat.SetValue (3, 0, static_cast<T> (0));
    theMat.SetValue (3, 1, static_cast<T> (0));
    theMat.SetValue (3, 2, static_cast<T> (0));
    theMat.SetValue (3, 3, static_cast<T> (1));
  }

friend class gp_GTrsf;

protected:

  //! Makes orthogonalization of "matrix"
  Standard_EXPORT void Orthogonalize();

private:

  Standard_Real scale;
  gp_TrsfForm shape;
  gp_Mat matrix;
  gp_XYZ loc;

};

#include <gp_Trsf.lxx>

#endif // _gp_Trsf_HeaderFile
