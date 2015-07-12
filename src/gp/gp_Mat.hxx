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

#ifndef _gp_Mat_HeaderFile
#define _gp_Mat_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_XYZ;
class gp_Trsf;
class gp_GTrsf;



//! Describes a three column, three row matrix. This sort of
//! object is used in various vectorial or matrix computations.
class gp_Mat 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates  a matrix with null coefficients.
    gp_Mat();
  
    gp_Mat(const Standard_Real a11, const Standard_Real a12, const Standard_Real a13, const Standard_Real a21, const Standard_Real a22, const Standard_Real a23, const Standard_Real a31, const Standard_Real a32, const Standard_Real a33);
  
  //! Creates a matrix.
  //! Col1, Col2, Col3 are the 3 columns of the matrix.
  Standard_EXPORT gp_Mat(const gp_XYZ& Col1, const gp_XYZ& Col2, const gp_XYZ& Col3);
  
  //! Assigns the three coordinates of Value to the column of index
  //! Col of this matrix.
  //! Raises OutOfRange if Col < 1 or Col > 3.
  Standard_EXPORT void SetCol (const Standard_Integer Col, const gp_XYZ& Value);
  
  //! Assigns the number triples Col1, Col2, Col3 to the three
  //! columns of this matrix.
  Standard_EXPORT void SetCols (const gp_XYZ& Col1, const gp_XYZ& Col2, const gp_XYZ& Col3);
  

  //! Modifies the matrix  M so that applying it to any number
  //! triple (X, Y, Z) produces the same result as the cross
  //! product of Ref and the number triple (X, Y, Z):
  //! i.e.: M * {X,Y,Z}t = Ref.Cross({X, Y ,Z})
  //! this matrix is anti symmetric. To apply this matrix to the
  //! triplet  {XYZ} is the same as to do the cross product between the
  //! triplet Ref and the triplet {XYZ}.
  //! Note: this matrix is anti-symmetric.
  Standard_EXPORT void SetCross (const gp_XYZ& Ref);
  

  //! Modifies the main diagonal of the matrix.
  //! <me>.Value (1, 1) = X1
  //! <me>.Value (2, 2) = X2
  //! <me>.Value (3, 3) = X3
  //! The other coefficients of the matrix are not modified.
    void SetDiagonal (const Standard_Real X1, const Standard_Real X2, const Standard_Real X3);
  

  //! Modifies this matrix so that applying it to any number
  //! triple (X, Y, Z) produces the same result as the scalar
  //! product of Ref and the number triple (X, Y, Z):
  //! this * (X,Y,Z) = Ref.(X,Y,Z)
  //! Note: this matrix is symmetric.
  Standard_EXPORT void SetDot (const gp_XYZ& Ref);
  
  //! Modifies this matrix so that it represents the Identity matrix.
    void SetIdentity();
  

  //! Modifies this matrix so that it represents a rotation. Ang is the angular value in
  //! radians and the XYZ axis gives the direction of the
  //! rotation.
  //! Raises ConstructionError if XYZ.Modulus() <= Resolution()
  Standard_EXPORT void SetRotation (const gp_XYZ& Axis, const Standard_Real Ang);
  
  //! Assigns the three coordinates of Value to the row of index
  //! Row of this matrix. Raises OutOfRange if Row < 1 or Row > 3.
  Standard_EXPORT void SetRow (const Standard_Integer Row, const gp_XYZ& Value);
  
  //! Assigns the number triples Row1, Row2, Row3 to the three
  //! rows of this matrix.
  Standard_EXPORT void SetRows (const gp_XYZ& Row1, const gp_XYZ& Row2, const gp_XYZ& Row3);
  

  //! Modifies the the matrix so that it represents
  //! a scaling transformation, where S is the scale factor. :
  //! | S    0.0  0.0 |
  //! <me> =  | 0.0   S   0.0 |
  //! | 0.0  0.0   S  |
    void SetScale (const Standard_Real S);
  
  //! Assigns <Value> to the coefficient of row Row, column Col of   this matrix.
  //! Raises OutOfRange if Row < 1 or Row > 3 or Col < 1 or Col > 3
    void SetValue (const Standard_Integer Row, const Standard_Integer Col, const Standard_Real Value);
  
  //! Returns the column of Col index.
  //! Raises OutOfRange if Col < 1 or Col > 3
  Standard_EXPORT gp_XYZ Column (const Standard_Integer Col) const;
  
  //! Computes the determinant of the matrix.
    Standard_Real Determinant() const;
  
  //! Returns the main diagonal of the matrix.
  Standard_EXPORT gp_XYZ Diagonal() const;
  
  //! returns the row of Row index.
  //! Raises OutOfRange if Row < 1 or Row > 3
  Standard_EXPORT gp_XYZ Row (const Standard_Integer Row) const;
  
  //! Returns the coefficient of range (Row, Col)
  //! Raises OutOfRange if Row < 1 or Row > 3 or Col < 1 or Col > 3
    const Standard_Real& Value (const Standard_Integer Row, const Standard_Integer Col) const;
  const Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col) const
{
  return Value(Row,Col);
}
  
  //! Returns the coefficient of range (Row, Col)
  //! Raises OutOfRange if Row < 1 or Row > 3 or Col < 1 or Col > 3
    Standard_Real& ChangeValue (const Standard_Integer Row, const Standard_Integer Col);
  Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col)
{
  return ChangeValue(Row,Col);
}
  

  //! The Gauss LU decomposition is used to invert the matrix
  //! (see Math package) so the matrix is considered as singular if
  //! the largest pivot found is lower or equal to Resolution from gp.
    Standard_Boolean IsSingular() const;
  
    void Add (const gp_Mat& Other);
  void operator += (const gp_Mat& Other)
{
  Add(Other);
}
  
  //! Computes the sum of this matrix and
  //! the matrix Other for each coefficient of the matrix :
  //! <me>.Coef(i,j) + <Other>.Coef(i,j)
    gp_Mat Added (const gp_Mat& Other) const;
  gp_Mat operator + (const gp_Mat& Other) const
{
  return Added(Other);
}
  
    void Divide (const Standard_Real Scalar);
  void operator /= (const Standard_Real Scalar)
{
  Divide(Scalar);
}
  
  //! Divides all the coefficients of the matrix by Scalar
    gp_Mat Divided (const Standard_Real Scalar) const;
  gp_Mat operator / (const Standard_Real Scalar) const
{
  return Divided(Scalar);
}
  
  Standard_EXPORT void Invert();
  

  //! Inverses the matrix and raises if the matrix is singular.
  //! -   Invert assigns the result to this matrix, while
  //! -   Inverted creates a new one.
  //! Warning
  //! The Gauss LU decomposition is used to invert the matrix.
  //! Consequently, the matrix is considered as singular if the
  //! largest pivot found is less than or equal to gp::Resolution().
  //! Exceptions
  //! Standard_ConstructionError if this matrix is singular,
  //! and therefore cannot be inverted.
  Standard_EXPORT gp_Mat Inverted() const;
  

  //! Computes  the product of two matrices <me> * <Other>
    gp_Mat Multiplied (const gp_Mat& Other) const;
  gp_Mat operator * (const gp_Mat& Other) const
{
  return Multiplied(Other);
}
  
  //! Computes the product of two matrices <me> = <Other> * <me>.
    void Multiply (const gp_Mat& Other);
  void operator *= (const gp_Mat& Other)
{
  Multiply(Other);
}
  
    void PreMultiply (const gp_Mat& Other);
  
    gp_Mat Multiplied (const Standard_Real Scalar) const;
  gp_Mat operator * (const Standard_Real Scalar) const
{
  return Multiplied(Scalar);
}
  

  //! Multiplies all the coefficients of the matrix by Scalar
    void Multiply (const Standard_Real Scalar);
  void operator *= (const Standard_Real Scalar)
{
  Multiply(Scalar);
}
  
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! Computes <me> = <me> * <me> * .......* <me>,   N time.
  //! if N = 0 <me> = Identity
  //! if N < 0 <me> = <me>.Invert() *...........* <me>.Invert().
  //! If N < 0 an exception will be raised if the matrix is not
  //! inversible
    gp_Mat Powered (const Standard_Integer N) const;
  
    void Subtract (const gp_Mat& Other);
  void operator -= (const gp_Mat& Other)
{
  Subtract(Other);
}
  

  //! cOmputes for each coefficient of the matrix :
  //! <me>.Coef(i,j) - <Other>.Coef(i,j)
    gp_Mat Subtracted (const gp_Mat& Other) const;
  gp_Mat operator - (const gp_Mat& Other) const
{
  return Subtracted(Other);
}
  
    void Transpose();
  

  //! Transposes the matrix. A(j, i) -> A (i, j)
    gp_Mat Transposed() const;


friend class gp_XYZ;
friend class gp_Trsf;
friend class gp_GTrsf;


protected:





private:



  Standard_Real matrix[3][3];


};


#include <gp_Mat.lxx>





#endif // _gp_Mat_HeaderFile
