// Created on: 1991-05-07
// Created by: Laurent PAINNOT
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

#ifndef _math_Matrix_HeaderFile
#define _math_Matrix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <NCollection_Allocator.hxx>

#include <math_DoubleTab.hxx>
#include <Standard_OStream.hxx>

// resolve name collisions with X11 headers
#ifdef Opposite
  #undef Opposite
#endif

template<typename T = double> class math_VectorBase;

//! This class implements the real matrix abstract data type.
//! Matrixes can have an arbitrary range which must be defined
//! at the declaration and cannot be changed after this declaration
//! math_Matrix(-3,5,2,4); //a vector with range [-3..5, 2..4]
//! Matrix values may be initialized and
//! retrieved using indexes which must lie within the range
//! of definition of the matrix.
//! Matrix objects follow "value semantics", that is, they
//! cannot be shared and are copied through assignment
//! Matrices are copied through assignment:
//! @code
//! math_Matrix M2(1, 9, 1, 3);
//! ...
//! M2 = M1;
//! M1(1) = 2.0;//the matrix M2 will not be modified.
//! @endcode
//! The exception RangeError is raised when trying to access
//! outside the range of a matrix :
//! @code
//! M1(11, 1)=0.0// --> will raise RangeError.
//! @endcode
//!
//! The exception DimensionError is raised when the dimensions of
//! two matrices or vectors are not compatible.
//! @code
//! math_Matrix M3(1, 2, 1, 2);
//! M3 = M1;   // will raise DimensionError
//! M1.Add(M3) // --> will raise DimensionError.
//! @endcode
//! A Matrix can be constructed with a pointer to "c array".
//! It allows to carry the bounds inside the matrix.
//! Example :
//! @code
//! Standard_Real tab1[10][20];
//! Standard_Real tab2[200];
//!
//! math_Matrix A (tab1[0][0], 1, 10, 1, 20);
//! math_Matrix B (tab2[0],    1, 10, 1, 20);
//! @endcode
class math_Matrix
{
public:

  DEFINE_STANDARD_ALLOC

  friend class math_VectorBase<>;

  //! Constructs a non-initialized  matrix of range [LowerRow..UpperRow,
  //! LowerCol..UpperCol]
  //! For the constructed matrix:
  //! -   LowerRow and UpperRow are the indexes of the
  //! lower and upper bounds of a row, and
  //! -   LowerCol and UpperCol are the indexes of the
  //! lower and upper bounds of a column.
  Standard_EXPORT math_Matrix(const Standard_Integer LowerRow, const Standard_Integer UpperRow, const Standard_Integer LowerCol, const Standard_Integer UpperCol);

  //! constructs a non-initialized matrix of range [LowerRow..UpperRow,
  //! LowerCol..UpperCol]
  //! whose values are all initialized with the value InitialValue.
  Standard_EXPORT math_Matrix(const Standard_Integer LowerRow, const Standard_Integer UpperRow, const Standard_Integer LowerCol, const Standard_Integer UpperCol, const Standard_Real InitialValue);

  //! constructs a matrix of range [LowerRow..UpperRow,
  //! LowerCol..UpperCol]
  //! Sharing data with a "C array" pointed by Tab.
  Standard_EXPORT math_Matrix(const Standard_Address Tab, const Standard_Integer LowerRow, const Standard_Integer UpperRow, const Standard_Integer LowerCol, const Standard_Integer UpperCol);

  //! constructs a matrix for copy in initialization.
  //! An exception is raised if the matrixes have not the same dimensions.
  Standard_EXPORT math_Matrix(const math_Matrix& Other);

  //! Initialize all the elements of a matrix to InitialValue.
  Standard_EXPORT void Init (const Standard_Real InitialValue);

  //! Returns the number of rows  of this matrix.
  //! Note that for a matrix A you always have the following relations:
  //! - A.RowNumber() = A.UpperRow() -   A.LowerRow() + 1
  //! - A.ColNumber() = A.UpperCol() -   A.LowerCol() + 1
  //! - the length of a row of A is equal to the number of columns of A,
  //! - the length of a column of A is equal to the number of
  //! rows of A.returns the row range of a matrix.
    Standard_Integer RowNumber() const;

  //! Returns the number of rows  of this matrix.
  //! Note that for a matrix A you always have the following relations:
  //! - A.RowNumber() = A.UpperRow() -   A.LowerRow() + 1
  //! - A.ColNumber() = A.UpperCol() -   A.LowerCol() + 1
  //! - the length of a row of A is equal to the number of columns of A,
  //! - the length of a column of A is equal to the number of
  //! rows of A.returns the row range of a matrix.
    Standard_Integer ColNumber() const;

  //! Returns the value of the Lower index of the row
  //! range of a matrix.
    Standard_Integer LowerRow() const;

  //! Returns the Upper index of the row range
  //! of a matrix.
    Standard_Integer UpperRow() const;

  //! Returns the value of the Lower index of the
  //! column range of a matrix.
    Standard_Integer LowerCol() const;

  //! Returns the value of the upper index of the
  //! column range of a matrix.
    Standard_Integer UpperCol() const;

  //! Computes the determinant of a matrix.
  //! An exception is raised if the matrix is not a square matrix.
  Standard_EXPORT Standard_Real Determinant() const;

  //! Transposes a given matrix.
  //! An exception is raised if the matrix is not a square matrix.
  Standard_EXPORT void Transpose();

  //! Inverts a matrix using Gauss algorithm.
  //! Exception NotSquare is raised if the matrix is not square.
  //! Exception SingularMatrix is raised if the matrix is singular.
  Standard_EXPORT void Invert();

  //! Sets this matrix to the product of the matrix Left, and the matrix Right.
  //! Example
  //! math_Matrix A (1, 3, 1, 3);
  //! math_Matrix B (1, 3, 1, 3);
  //! // A = ... , B = ...
  //! math_Matrix C (1, 3, 1, 3);
  //! C.Multiply(A, B);
  //! Exceptions
  //! Standard_DimensionError if matrices are of incompatible dimensions, i.e. if:
  //! -   the number of columns of matrix Left, or the number of
  //! rows of matrix TLeft is not equal to the number of rows
  //! of matrix Right, or
  //! -   the number of rows of matrix Left, or the number of
  //! columns of matrix TLeft is not equal to the number of
  //! rows of this matrix, or
  //! -   the number of columns of matrix Right is not equal to
  //! the number of columns of this matrix.
  Standard_EXPORT void Multiply (const Standard_Real Right);
void operator*= (const Standard_Real Right)
{
  Multiply(Right);
}

  //! multiplies all the elements of a matrix by the
  //! value <Right>.
  Standard_NODISCARD Standard_EXPORT math_Matrix Multiplied (const Standard_Real Right) const;
Standard_NODISCARD math_Matrix operator* (const Standard_Real Right) const
{
  return Multiplied(Right);
}

  //! Sets this matrix to the product of the
  //! transposed matrix TLeft, and the matrix Right.
  //! Example
  //! math_Matrix A (1, 3, 1, 3);
  //! math_Matrix B (1, 3, 1, 3);
  //! // A = ... , B = ...
  //! math_Matrix C (1, 3, 1, 3);
  //! C.Multiply(A, B);
  //! Exceptions
  //! Standard_DimensionError if matrices are of incompatible dimensions, i.e. if:
  //! -   the number of columns of matrix Left, or the number of
  //! rows of matrix TLeft is not equal to the number of rows
  //! of matrix Right, or
  //! -   the number of rows of matrix Left, or the number of
  //! columns of matrix TLeft is not equal to the number of
  //! rows of this matrix, or
  //! -   the number of columns of matrix Right is not equal to
  //! the number of columns of this matrix.
  Standard_NODISCARD Standard_EXPORT math_Matrix TMultiplied (const Standard_Real Right) const;
friend math_Matrix  operator *(const Standard_Real Left,const math_Matrix& Right);

  //! divides all the elements of a matrix by the value <Right>.
  //! An exception is raised if <Right> = 0.
  Standard_EXPORT void Divide (const Standard_Real Right);
void operator/= (const Standard_Real Right)
{
  Divide(Right);
}

  //! divides all the elements of a matrix by the value <Right>.
  //! An exception is raised if <Right> = 0.
  Standard_NODISCARD Standard_EXPORT math_Matrix Divided (const Standard_Real Right) const;
Standard_NODISCARD math_Matrix operator/ (const Standard_Real Right) const
{
  return Divided(Right);
}

  //! adds the matrix <Right> to a matrix.
  //! An exception is raised if the dimensions are different.
  //! Warning
  //! In order to save time when copying matrices, it is
  //! preferable to use operator += or the function Add
  //! whenever possible.
  Standard_EXPORT void Add (const math_Matrix& Right);
void operator+= (const math_Matrix& Right)
{
  Add(Right);
}

  //! adds the matrix <Right> to a matrix.
  //! An exception is raised if the dimensions are different.
  Standard_NODISCARD Standard_EXPORT math_Matrix Added (const math_Matrix& Right) const;
Standard_NODISCARD math_Matrix operator+ (const math_Matrix& Right) const
{
  return Added(Right);
}

  //! sets a  matrix to the addition of <Left> and <Right>.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT void Add (const math_Matrix& Left, const math_Matrix& Right);

  //! Subtracts the matrix <Right> from <me>.
  //! An exception is raised if the dimensions are different.
  //! Warning
  //! In order to avoid time-consuming copying of matrices, it
  //! is preferable to use operator -= or the function
  //! Subtract whenever possible.
  Standard_EXPORT void Subtract (const math_Matrix& Right);
void operator-= (const math_Matrix& Right)
{
  Subtract(Right);
}

  //! Returns the result of the subtraction of <Right> from <me>.
  //! An exception is raised if the dimensions are different.
  Standard_NODISCARD Standard_EXPORT math_Matrix Subtracted (const math_Matrix& Right) const;
Standard_NODISCARD math_Matrix operator- (const math_Matrix& Right) const
{
  return Subtracted(Right);
}

  //! Sets the values of this matrix,
  //! -   from index I1 to index I2 on the row dimension, and
  //! -   from index J1 to index J2 on the column dimension,
  //! to those of matrix M.
  //! Exceptions
  //! Standard_DimensionError if:
  //! -   I1 is less than the index of the lower row bound of this matrix, or
  //! -   I2 is greater than the index of the upper row bound of this matrix, or
  //! -   J1 is less than the index of the lower column bound of this matrix, or
  //! -   J2 is greater than the index of the upper column bound of this matrix, or
  //! -   I2 - I1 + 1 is not equal to the number of rows of matrix M, or
  //! -   J2 - J1 + 1 is not equal to the number of columns of matrix M.
  Standard_EXPORT void Set (const Standard_Integer I1, const Standard_Integer I2, const Standard_Integer J1, const Standard_Integer J2, const math_Matrix& M);

  //! Sets the row of index Row of a matrix to the vector <V>.
  //! An exception is raised if the dimensions are different.
  //! An exception is raises if <Row> is inferior to the lower
  //! row of the matrix or <Row> is superior to the upper row.
  Standard_EXPORT void SetRow (const Standard_Integer Row, const math_VectorBase<>& V);

  //! Sets the column of index Col of a matrix to the vector <V>.
  //! An exception is raised if the dimensions are different.
  //! An exception is raises if <Col> is inferior to the lower
  //! column of the matrix or <Col> is superior to the upper
  //! column.
  Standard_EXPORT void SetCol (const Standard_Integer Col, const math_VectorBase<>& V);

  //! Sets the diagonal of a matrix to the value <Value>.
  //! An exception is raised if the matrix is not square.
  Standard_EXPORT void SetDiag (const Standard_Real Value);

  //! Returns the row of index Row of a matrix.
  Standard_EXPORT math_VectorBase<> Row (const Standard_Integer Row) const;

  //! Returns the column of index <Col> of a matrix.
  Standard_EXPORT math_VectorBase<> Col (const Standard_Integer Col) const;

  //! Swaps the rows of index Row1 and Row2.
  //! An exception is raised if <Row1> or <Row2> is out of range.
  Standard_EXPORT void SwapRow (const Standard_Integer Row1, const Standard_Integer Row2);

  //! Swaps the columns of index <Col1> and <Col2>.
  //! An exception is raised if <Col1> or <Col2> is out of range.
  Standard_EXPORT void SwapCol (const Standard_Integer Col1, const Standard_Integer Col2);

  //! Teturns the transposed of a matrix.
  //! An exception is raised if the matrix is not a square matrix.
  Standard_NODISCARD Standard_EXPORT math_Matrix Transposed() const;

  //! Returns the inverse of a matrix.
  //! Exception NotSquare is raised if the matrix is not square.
  //! Exception SingularMatrix is raised if the matrix is singular.
  Standard_EXPORT math_Matrix Inverse() const;

  //! Returns the product of the transpose of a matrix with
  //! the matrix <Right>.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT math_Matrix TMultiply (const math_Matrix& Right) const;

  //! Computes a matrix as the product of 2 vectors.
  //! An exception is raised if the dimensions are different.
  //! <me> = <Left> * <Right>.
  Standard_EXPORT void Multiply (const math_VectorBase<>& Left, const math_VectorBase<>& Right);

  //! Computes a matrix as the product of 2 matrixes.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT void Multiply (const math_Matrix& Left, const math_Matrix& Right);

  //! Computes a matrix to the product of the transpose of
  //! the matrix <TLeft> with the matrix <Right>.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT void TMultiply (const math_Matrix& TLeft, const math_Matrix& Right);

  //! Sets a matrix to the Subtraction of the matrix <Right>
  //! from the matrix <Left>.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT void Subtract (const math_Matrix& Left, const math_Matrix& Right);

  //! Accesses (in read or write mode) the value of index <Row>
  //! and <Col> of a matrix.
  //! An exception is raised if <Row> and <Col> are not
  //! in the correct range.
    Standard_Real& Value (const Standard_Integer Row, const Standard_Integer Col) const;
  Standard_Real& operator() (const Standard_Integer Row, const Standard_Integer Col) const
{
  return Value(Row,Col);
}

  //! Matrixes are copied through assignment.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT math_Matrix& Initialized (const math_Matrix& Other);
math_Matrix& operator= (const math_Matrix& Other)
{
  return Initialized(Other);
}

  //! Returns the product of 2 matrices.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT void Multiply (const math_Matrix& Right);
void operator*= (const math_Matrix& Right)
{
  Multiply(Right);
}

  //! Returns the product of 2 matrices.
  //! An exception is raised if the dimensions are different.
  Standard_NODISCARD Standard_EXPORT math_Matrix Multiplied (const math_Matrix& Right) const;
Standard_NODISCARD math_Matrix operator* (const math_Matrix& Right) const
{
  return Multiplied(Right);
}

  //! Returns the product of a matrix by a vector.
  //! An exception is raised if the dimensions are different.
  Standard_NODISCARD Standard_EXPORT math_VectorBase<> Multiplied (const math_VectorBase<>& Right) const;
  Standard_NODISCARD Standard_EXPORT math_VectorBase<> operator* (const math_VectorBase<>& Right) const;

  //! Returns the opposite of a matrix.
  //! An exception is raised if the dimensions are different.
  Standard_EXPORT math_Matrix Opposite();
math_Matrix operator-()
{
  return Opposite();
}

  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;

protected:


  //! The new lower row of the matrix is set to <LowerRow>
  Standard_EXPORT void SetLowerRow (const Standard_Integer LowerRow);

  //! The new lower column of the matrix is set to the column
  //! of range <LowerCol>.
  Standard_EXPORT void SetLowerCol (const Standard_Integer LowerCol);

  //! The new lower row of the matrix is set to <LowerRow>
  //! and the new lower column of the matrix is set to the column
  //! of range <LowerCol>.
  void SetLower (const Standard_Integer LowerRow, const Standard_Integer LowerCol);




private:



  Standard_Integer LowerRowIndex;
  Standard_Integer UpperRowIndex;
  Standard_Integer LowerColIndex;
  Standard_Integer UpperColIndex;
  math_DoubleTab Array;


};


#include <math_Matrix.lxx>





#endif // _math_Matrix_HeaderFile
