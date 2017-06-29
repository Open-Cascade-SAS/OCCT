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

#ifndef _gp_GTrsf_HeaderFile
#define _gp_GTrsf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
#include <gp_TrsfForm.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Trsf.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Trsf;
class gp_Mat;
class gp_XYZ;
class gp_Ax1;
class gp_Ax2;

// Avoid possible conflict with SetForm macro defined by windows.h
#ifdef SetForm
#undef SetForm
#endif

//! Defines a non-persistent transformation in 3D space.
//! This transformation is a general transformation.
//! It can be a Trsf from gp, an affinity, or you can define
//! your own transformation giving the matrix of transformation.
//!
//! With a Gtrsf you can transform only a triplet of coordinates
//! XYZ. It is not possible to transform other geometric objects
//! because these transformations can change the nature of non-
//! elementary geometric objects.
//! The transformation GTrsf can be represented as follow :
//!
//! V1   V2   V3    T       XYZ        XYZ
//! | a11  a12  a13   a14 |   | x |      | x'|
//! | a21  a22  a23   a24 |   | y |      | y'|
//! | a31  a32  a33   a34 |   | z |   =  | z'|
//! |  0    0    0     1  |   | 1 |      | 1 |
//!
//! where {V1, V2, V3} define the vectorial part of the
//! transformation and T defines the translation part of the
//! transformation.
//! Warning
//! A GTrsf transformation is only applicable to
//! coordinates. Be careful if you apply such a
//! transformation to all points of a geometric object, as
//! this can change the nature of the object and thus
//! render it incoherent!
//! Typically, a circle is transformed into an ellipse by an
//! affinity transformation. To avoid modifying the nature of
//! an object, use a gp_Trsf transformation instead, as
//! objects of this class respect the nature of geometric objects.
class gp_GTrsf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the Identity transformation.
    gp_GTrsf();
  

  //! Converts the gp_Trsf transformation T into a
  //! general transformation, i.e. Returns a GTrsf with
  //! the same matrix of coefficients as the Trsf T.
    gp_GTrsf(const gp_Trsf& T);
  

  //! Creates a transformation based on the matrix M and the
  //! vector V where M defines the vectorial part of
  //! the transformation, and V the translation part, or
    gp_GTrsf(const gp_Mat& M, const gp_XYZ& V);
  
  //! Changes this transformation into an affinity of ratio Ratio
  //! with respect to the axis A1.
  //! Note: an affinity is a point-by-point transformation that
  //! transforms any point P into a point P' such that if H is
  //! the orthogonal projection of P on the axis A1 or the
  //! plane A2, the vectors HP and HP' satisfy:
  //! HP' = Ratio * HP.
    void SetAffinity (const gp_Ax1& A1, const Standard_Real Ratio);
  
  //! Changes this transformation into an affinity of ratio Ratio
  //! with respect to  the plane defined by the origin, the "X Direction" and
  //! the "Y Direction" of coordinate system A2.
  //! Note: an affinity is a point-by-point transformation that
  //! transforms any point P into a point P' such that if H is
  //! the orthogonal projection of P on the axis A1 or the
  //! plane A2, the vectors HP and HP' satisfy:
  //! HP' = Ratio * HP.
    void SetAffinity (const gp_Ax2& A2, const Standard_Real Ratio);
  

  //! Replaces  the coefficient (Row, Col) of the matrix representing
  //! this transformation by Value.  Raises OutOfRange
  //! if  Row < 1 or Row > 3 or Col < 1 or Col > 4
    void SetValue (const Standard_Integer Row, const Standard_Integer Col, const Standard_Real Value);
  
  //! Replaces the vectorial part of this transformation by Matrix.
    void SetVectorialPart (const gp_Mat& Matrix);
  
  //! Replaces the translation part of
  //! this transformation by the coordinates of the number triple Coord.
  Standard_EXPORT void SetTranslationPart (const gp_XYZ& Coord);
  
  //! Assigns the vectorial and translation parts of T to this transformation.
    void SetTrsf (const gp_Trsf& T);
  

  //! Returns true if the determinant of the vectorial part of
  //! this transformation is negative.
    Standard_Boolean IsNegative() const;
  

  //! Returns true if this transformation is singular (and
  //! therefore, cannot be inverted).
  //! Note: The Gauss LU decomposition is used to invert the
  //! transformation matrix. Consequently, the transformation
  //! is considered as singular if the largest pivot found is less
  //! than or equal to gp::Resolution().
  //! Warning
  //! If this transformation is singular, it cannot be inverted.
    Standard_Boolean IsSingular() const;
  

  //! Returns the nature of the transformation.  It can be an
  //! identity transformation, a rotation, a translation, a mirror
  //! transformation (relative to a point, an axis or a plane), a
  //! scaling transformation, a compound transformation or
  //! some other type of transformation.
  gp_TrsfForm Form() const;
  

  //! verify and set the shape of the GTrsf Other or CompoundTrsf
  //! Ex :
  //! myGTrsf.SetValue(row1,col1,val1);
  //! myGTrsf.SetValue(row2,col2,val2);
  //! ...
  //! myGTrsf.SetForm();
  Standard_EXPORT void SetForm();
  
  //! Returns the translation part of the GTrsf.
    const gp_XYZ& TranslationPart() const;
  

  //! Computes the vectorial part of the GTrsf. The returned Matrix
  //! is a  3*3 matrix.
    const gp_Mat& VectorialPart() const;
  

  //! Returns the coefficients of the global matrix of transformation.
  //! Raises OutOfRange if Row < 1 or Row > 3 or Col < 1 or Col > 4
    Standard_Real Value (const Standard_Integer Row, const Standard_Integer Col) const;
  Standard_Real operator() (const Standard_Integer Row, const Standard_Integer Col) const
{
  return Value(Row,Col);
}
  
  Standard_EXPORT void Invert();
  

  //! Computes the reverse transformation.
  //! Raises an exception if the matrix of the transformation
  //! is not inversible.
    gp_GTrsf Inverted() const;
  

  //! Computes the transformation composed from T and <me>.
  //! In a C++ implementation you can also write Tcomposed = <me> * T.
  //! Example :
  //! GTrsf T1, T2, Tcomp; ...............
  //! //composition :
  //! Tcomp = T2.Multiplied(T1);         // or   (Tcomp = T2 * T1)
  //! // transformation of a point
  //! XYZ P(10.,3.,4.);
  //! XYZ P1(P);
  //! Tcomp.Transforms(P1);               //using Tcomp
  //! XYZ P2(P);
  //! T1.Transforms(P2);                  //using T1 then T2
  //! T2.Transforms(P2);                  // P1 = P2 !!!
    gp_GTrsf Multiplied (const gp_GTrsf& T) const;
  gp_GTrsf operator * (const gp_GTrsf& T)  const
  {
    return Multiplied(T);
  }
  

  //! Computes the transformation composed with <me> and T.
  //! <me> = <me> * T
  Standard_EXPORT void Multiply (const gp_GTrsf& T);
  void operator *= (const gp_GTrsf& T) 
  {
    Multiply(T);
  }
  

  //! Computes the product of the transformation T and this
  //! transformation and assigns the result to this transformation.
  //! this = T * this
  Standard_EXPORT void PreMultiply (const gp_GTrsf& T);
  
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Computes:
  //! -   the product of this transformation multiplied by itself
  //! N times, if N is positive, or
  //! -   the product of the inverse of this transformation
  //! multiplied by itself |N| times, if N is negative.
  //! If N equals zero, the result is equal to the Identity
  //! transformation.
  //! I.e.:  <me> * <me> * .......* <me>, N time.
  //! if N =0 <me> = Identity
  //! if N < 0 <me> = <me>.Inverse() *...........* <me>.Inverse().
  //!
  //! Raises an exception if N < 0 and if the matrix of the
  //! transformation not inversible.
    gp_GTrsf Powered (const Standard_Integer N) const;
  
    void Transforms (gp_XYZ& Coord) const;
  
  //! Transforms a triplet XYZ with a GTrsf.
    void Transforms (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;
  
    gp_Trsf Trsf() const;




protected:





private:



  gp_Mat matrix;
  gp_XYZ loc;
  gp_TrsfForm shape;
  Standard_Real scale;


};


#include <gp_GTrsf.lxx>





#endif // _gp_GTrsf_HeaderFile
