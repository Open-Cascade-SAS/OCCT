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

#ifndef _gp_GTrsf2d_HeaderFile
#define _gp_GTrsf2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Mat2d.hxx>
#include <gp_XY.hxx>
#include <gp_TrsfForm.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Trsf2d;
class gp_Mat2d;
class gp_XY;
class gp_Ax2d;



//! Defines a non persistent transformation in 2D space.
//! This transformation is a general transformation.
//! It can be a Trsf2d from package gp, an affinity, or you can
//! define your own transformation giving the corresponding
//! matrix of transformation.
//!
//! With a GTrsf2d you can transform only a doublet of coordinates
//! XY. It is not possible to transform other geometric objects
//! because these transformations can change the nature of non-
//! elementary geometric objects.
//! A GTrsf2d is represented with a 2 rows * 3 columns matrix :
//!
//! V1   V2   T        XY         XY
//! | a11  a12  a14 |   | x |      | x'|
//! | a21  a22  a24 |   | y |      | y'|
//! |  0    0    1  |   | 1 |      | 1 |
//!
//! where {V1, V2} defines the vectorial part of the
//! transformation and T defines the translation part of
//! the transformation.
//! Warning
//! A GTrsf2d transformation is only applicable on
//! coordinates. Be careful if you apply such a
//! transformation to all the points of a geometric object,
//! as this can change the nature of the object and thus
//! render it incoherent!
//! Typically, a circle is transformed into an ellipse by an
//! affinity transformation. To avoid modifying the nature of
//! an object, use a gp_Trsf2d transformation instead, as
//! objects of this class respect the nature of geometric objects.
class gp_GTrsf2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns identity transformation.
    gp_GTrsf2d();
  
  //! Converts the gp_Trsf2d transformation T into a
  //! general transformation.
    gp_GTrsf2d(const gp_Trsf2d& T);
  
  //! Creates   a transformation based on the matrix M and the
  //! vector V where M defines the vectorial part of the
  //! transformation, and V the translation part.
    gp_GTrsf2d(const gp_Mat2d& M, const gp_XY& V);
  

  //! Changes this transformation into an affinity of ratio Ratio
  //! with respect to the axis A.
  //! Note: An affinity is a point-by-point transformation that
  //! transforms any point P into a point P' such that if H is
  //! the orthogonal projection of P on the axis A, the vectors
  //! HP and HP' satisfy: HP' = Ratio * HP.
  Standard_EXPORT void SetAffinity (const gp_Ax2d& A, const Standard_Real Ratio);
  

  //! Replaces   the coefficient (Row, Col) of the matrix representing
  //! this transformation by Value,
  //! Raises OutOfRange if Row < 1 or Row > 2 or Col < 1 or Col > 3
    void SetValue (const Standard_Integer Row, const Standard_Integer Col, const Standard_Real Value);
  
  //! Replacesthe translation part of this
  //! transformation by the coordinates of the number pair Coord.
  Standard_EXPORT void SetTranslationPart (const gp_XY& Coord);
  

  //! Assigns the vectorial and translation parts of T to this transformation.
    void SetTrsf2d (const gp_Trsf2d& T);
  

  //! Replaces the vectorial part of this transformation by Matrix.
    void SetVectorialPart (const gp_Mat2d& Matrix);
  

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
  

  //! Returns the nature of the transformation.  It can be
  //! an identity transformation, a rotation, a translation, a mirror
  //! transformation (relative to a point or axis), a scaling
  //! transformation, a compound transformation or some
  //! other type of transformation.
    gp_TrsfForm Form() const;
  
  //! Returns the translation part of the GTrsf2d.
    const gp_XY& TranslationPart() const;
  

  //! Computes the vectorial part of the GTrsf2d. The returned
  //! Matrix is a 2*2 matrix.
    const gp_Mat2d& VectorialPart() const;
  

  //! Returns the coefficients of the global matrix of transformation.
  //! Raised OutOfRange if Row < 1 or Row > 2 or Col < 1 or Col > 3
    Standard_Real Value (const Standard_Integer Row, const Standard_Integer Col) const;
  Standard_Real operator() (const Standard_Integer Row, const Standard_Integer Col) const
{
  return Value(Row,Col);
}
  
  Standard_EXPORT void Invert();
  

  //! Computes the reverse transformation.
  //! Raised an exception if the matrix of the transformation
  //! is not inversible.
    gp_GTrsf2d Inverted() const;
  

  //! Computes the transformation composed with T and <me>.
  //! In a C++ implementation you can also write Tcomposed = <me> * T.
  //! Example :
  //! GTrsf2d T1, T2, Tcomp; ...............
  //! //composition :
  //! Tcomp = T2.Multiplied(T1);         // or   (Tcomp = T2 * T1)
  //! // transformation of a point
  //! XY P(10.,3.);
  //! XY P1(P);
  //! Tcomp.Transforms(P1);               //using Tcomp
  //! XY P2(P);
  //! T1.Transforms(P2);                  //using T1 then T2
  //! T2.Transforms(P2);                  // P1 = P2 !!!
    gp_GTrsf2d Multiplied (const gp_GTrsf2d& T) const;
  gp_GTrsf2d operator * (const gp_GTrsf2d& T) const
{
  return Multiplied(T);
}
  
  Standard_EXPORT void Multiply (const gp_GTrsf2d& T);
void operator *= (const gp_GTrsf2d& T)
{
  Multiply(T);
}
  

  //! Computes the product of the transformation T and this
  //! transformation, and assigns the result to this transformation:
  //! this = T * this
  Standard_EXPORT void PreMultiply (const gp_GTrsf2d& T);
  
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Computes the following composition of transformations
  //! <me> * <me> * .......* <me>, N time.
  //! if N = 0 <me> = Identity
  //! if N < 0 <me> = <me>.Inverse() *...........* <me>.Inverse().
  //!
  //! Raises an exception if N < 0 and if the matrix of the
  //! transformation is not inversible.
    gp_GTrsf2d Powered (const Standard_Integer N) const;
  
    void Transforms (gp_XY& Coord) const;
  
    gp_XY Transformed (const gp_XY& Coord) const;
  

  //! Applies this transformation to the coordinates:
  //! -   of the number pair Coord, or
  //! -   X and Y.
  //!
  //! Note:
  //! -   Transforms modifies X, Y, or the coordinate pair Coord, while
  //! -   Transformed creates a new coordinate pair.
    void Transforms (Standard_Real& X, Standard_Real& Y) const;
  

  //! Converts this transformation into a gp_Trsf2d transformation.
  //! Exceptions
  //! Standard_ConstructionError if this transformation
  //! cannot be converted, i.e. if its form is gp_Other.
  Standard_EXPORT gp_Trsf2d Trsf2d() const;




protected:





private:



  gp_Mat2d matrix;
  gp_XY loc;
  gp_TrsfForm shape;
  Standard_Real scale;


};


#include <gp_GTrsf2d.lxx>





#endif // _gp_GTrsf2d_HeaderFile
