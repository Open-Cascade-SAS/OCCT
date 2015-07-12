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

#ifndef _gp_Mat2d_HeaderFile
#define _gp_Mat2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Trsf2d;
class gp_GTrsf2d;
class gp_XY;



//! Describes a two column, two row matrix. This sort of
//! object is used in various vectorial or matrix computations.
class gp_Mat2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates  a matrix with null coefficients.
    gp_Mat2d();
  

  //! Col1, Col2 are the 2 columns of the matrix.
  Standard_EXPORT gp_Mat2d(const gp_XY& Col1, const gp_XY& Col2);
  
  //! Assigns the two coordinates of Value to the column of range
  //! Col of this matrix
  //! Raises OutOfRange if Col < 1 or Col > 2.
  Standard_EXPORT void SetCol (const Standard_Integer Col, const gp_XY& Value);
  
  //! Assigns the number pairs Col1, Col2 to the two columns of   this matrix
  Standard_EXPORT void SetCols (const gp_XY& Col1, const gp_XY& Col2);
  

  //! Modifies the main diagonal of the matrix.
  //! <me>.Value (1, 1) = X1
  //! <me>.Value (2, 2) = X2
  //! The other coefficients of the matrix are not modified.
    void SetDiagonal (const Standard_Real X1, const Standard_Real X2);
  
  //! Modifies this matrix, so that it represents the Identity matrix.
    void SetIdentity();
  

  //! Modifies this matrix, so that it representso a rotation. Ang is the angular
  //! value in radian of the rotation.
    void SetRotation (const Standard_Real Ang);
  
  //! Assigns the two coordinates of Value to the row of index Row of this matrix.
  //! Raises OutOfRange if Row < 1 or Row > 2.
  Standard_EXPORT void SetRow (const Standard_Integer Row, const gp_XY& Value);
  
  //! Assigns the number pairs Row1, Row2 to the two rows of this matrix.
  Standard_EXPORT void SetRows (const gp_XY& Row1, const gp_XY& Row2);
  

  //! Modifies the matrix such that it
  //! represents a scaling transformation, where S is the scale   factor :
  //! | S    0.0 |
  //! <me> =  | 0.0   S  |
    void SetScale (const Standard_Real S);
  
  //! Assigns <Value> to the coefficient of row Row, column Col of this matrix.
  //! Raises OutOfRange if Row < 1 or Row > 2 or Col < 1 or Col > 2
    void SetValue (const Standard_Integer Row, const Standard_Integer Col, const Standard_Real Value);
  
  //! Returns the column of Col index.
  //! Raises OutOfRange if Col < 1 or Col > 2
  Standard_EXPORT gp_XY Column (const Standard_Integer Col) const;
  
  //! Computes the determinant of the matrix.
    Standard_Real Determinant() const;
  
  //! Returns the main diagonal of the matrix.
  Standard_EXPORT gp_XY Diagonal() const;
  
  //! Returns the row of index Row.
  //! Raised if Row < 1 or Row > 2
  Standard_EXPORT gp_XY Row (const Standard_Integer Row) const;
  
  //! Returns the coefficient of range (Row, Col)
  //! Raises OutOfRange
  //! if Row < 1 or Row > 2 or Col < 1 or Col > 2
    const Standard_Real& Value (const Standard_Integer Row, const Standard_Integer Col) const;
  const Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col) const
{
  return Value(Row,Col);
}
  
  //! Returns the coefficient of range (Row, Col)
  //! Raises OutOfRange
  //! if Row < 1 or Row > 2 or Col < 1 or Col > 2
    Standard_Real& ChangeValue (const Standard_Integer Row, const Standard_Integer Col);
  Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col)
{
  return ChangeValue(Row,Col);
}
  

  //! Returns true if this matrix is singular (and therefore, cannot be inverted).
  //! The Gauss LU decomposition is used to invert the matrix
  //! so the matrix is considered as singular if the largest
  //! pivot found is lower or equal to Resolution from gp.
    Standard_Boolean IsSingular() const;
  
    void Add (const gp_Mat2d& Other);
  void operator += (const gp_Mat2d& Other)
{
  Add(Other);
}
  

  //! Computes the sum of this matrix and the matrix
  //! Other.for each coefficient of the matrix :
  //! <me>.Coef(i,j) + <Other>.Coef(i,j)
  //! Note:
  //! -   operator += assigns the result to this matrix, while
  //! -   operator + creates a new one.
    gp_Mat2d Added (const gp_Mat2d& Other) const;
  gp_Mat2d operator + (const gp_Mat2d& Other) const
{
  return Added(Other);
}
  
    void Divide (const Standard_Real Scalar);
  void operator /= (const Standard_Real Scalar)
{
  Divide(Scalar);
}
  

  //! Divides all the coefficients of the matrix by a scalar.
    gp_Mat2d Divided (const Standard_Real Scalar) const;
  gp_Mat2d operator / (const Standard_Real Scalar) const
{
  return Divided(Scalar);
}
  
  Standard_EXPORT void Invert();
  

  //! Inverses the matrix and raises exception if the matrix
  //! is singular.
    gp_Mat2d Inverted() const;
  
    gp_Mat2d Multiplied (const gp_Mat2d& Other) const;
  gp_Mat2d operator * (const gp_Mat2d& Other) const
{
  return Multiplied(Other);
}
  

  //! Computes the product of two matrices <me> * <Other>
    void Multiply (const gp_Mat2d& Other);
  
  //! Modifies this matrix by premultiplying it by the matrix Other
  //! <me> = Other * <me>.
    void PreMultiply (const gp_Mat2d& Other);
  
    gp_Mat2d Multiplied (const Standard_Real Scalar) const;
  gp_Mat2d operator * (const Standard_Real Scalar) const
{
  return Multiplied(Scalar);
}
  

  //! Multiplies all the coefficients of the matrix by a scalar.
    void Multiply (const Standard_Real Scalar);
  void operator *= (const Standard_Real Scalar)
{
  Multiply(Scalar);
}
  
  Standard_EXPORT void Power (const Standard_Integer N);
  

  //! computes <me> = <me> * <me> * .......* <me>, N time.
  //! if N = 0 <me> = Identity
  //! if N < 0 <me> = <me>.Invert() *...........* <me>.Invert().
  //! If N < 0 an exception can be raised if the matrix is not
  //! inversible
    gp_Mat2d Powered (const Standard_Integer N) const;
  
    void Subtract (const gp_Mat2d& Other);
  void operator -= (const gp_Mat2d& Other)
{
  Subtract(Other);
}
  

  //! Computes for each coefficient of the matrix :
  //! <me>.Coef(i,j) - <Other>.Coef(i,j)
    gp_Mat2d Subtracted (const gp_Mat2d& Other) const;
  gp_Mat2d operator - (const gp_Mat2d& Other) const
{
  return Subtracted(Other);
}
  
    void Transpose();
  

  //! Transposes the matrix. A(j, i) -> A (i, j)
    gp_Mat2d Transposed() const;


friend class gp_Trsf2d;
friend class gp_GTrsf2d;
friend class gp_XY;


protected:





private:



  Standard_Real matrix[2][2];


};


#include <gp_Mat2d.lxx>





#endif // _gp_Mat2d_HeaderFile
