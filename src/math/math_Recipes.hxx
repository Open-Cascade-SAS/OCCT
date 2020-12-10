// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef math_Recipes_HeaderFile
#define math_Recipes_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Handle.hxx>
#include <Message_ProgressRange.hxx>

class math_IntegerVector;
class math_Vector;
class math_Matrix;

const Standard_Integer math_Status_UserAborted         = -1;
const Standard_Integer math_Status_OK                  = 0;
const Standard_Integer math_Status_SingularMatrix      = 1;
const Standard_Integer math_Status_ArgumentError       = 2;
const Standard_Integer math_Status_NoConvergence       = 3;

Standard_EXPORT Standard_Integer  LU_Decompose(math_Matrix& a, 
                                        math_IntegerVector& indx,
                                        Standard_Real&   d,
                                        Standard_Real    TINY = 1.0e-20,
                                        const Message_ProgressRange& theProgress = Message_ProgressRange());

// Given a matrix a(1..n, 1..n), this routine computes its LU decomposition, 
// The matrix a is replaced by this LU decomposition and the vector indx(1..n)
// is an output which records the row permutation effected by the partial
// pivoting; d is output as +1 or -1 depending on whether the number of row
// interchanges was even or odd.

Standard_EXPORT Standard_Integer LU_Decompose(math_Matrix& a, 
                                        math_IntegerVector& indx,
                                        Standard_Real&   d,
                                        math_Vector& vv,
                                        Standard_Real    TINY = 1.0e-30,
                                        const Message_ProgressRange& theProgress = Message_ProgressRange());

// Idem to the previous LU_Decompose function. But the input Vector vv(1..n) is
// used internally as a scratch area.


Standard_EXPORT void LU_Solve(const math_Matrix& a,
              const math_IntegerVector& indx, 
              math_Vector& b);

// Solves a * x = b for a vector x, where x is specified by a(1..n, 1..n),
// indx(1..n) as returned by LU_Decompose. n is the dimension of the 
// square matrix A. b(1..n) is the input right-hand side and will be 
// replaced by the solution vector.Neither a and indx are destroyed, so 
// the routine may be called sequentially with different b's.


Standard_EXPORT Standard_Integer LU_Invert(math_Matrix& a);

// Given a matrix a(1..n, 1..n) this routine computes its inverse. The matrix
// a is replaced by its inverse.


Standard_EXPORT Standard_Integer SVD_Decompose(math_Matrix& a,
					  math_Vector& w,                    
					  math_Matrix& v);

// Given a matrix a(1..m, 1..n), this routine computes its singular value 
// decomposition, a = u * w * transposed(v). The matrix u replaces a on 
// output. The diagonal matrix of singular values w is output as a vector 
// w(1..n). The matrix v is output as v(1..n, 1..n). m must be greater or
// equal to n; if it is smaller, then a should be filled up to square with
// zero rows.


Standard_EXPORT Standard_Integer SVD_Decompose(math_Matrix& a,
					  math_Vector& w,
					  math_Matrix& v,
					  math_Vector& rv1);


// Idem to the previous LU_Decompose function. But the input Vector vv(1..m) 
// (the number of rows a(1..m, 1..n)) is used internally as a scratch area.


Standard_EXPORT void SVD_Solve(const math_Matrix& u,
			  const math_Vector& w,
			  const math_Matrix& v,
			  const math_Vector& b,
			  math_Vector& x);

// Solves a * x = b for a vector x, where x is specified by u(1..m, 1..n),
// w(1..n), v(1..n, 1..n) as returned by SVD_Decompose. m and n are the 
// dimensions of A, and will be equal for square matrices. b(1..m) is the 
// input right-hand side. x(1..n) is the output solution vector.
// No input quantities are destroyed, so the routine may be called 
// sequentially with different b's.



Standard_EXPORT Standard_Integer DACTCL_Decompose(math_Vector& a, const math_IntegerVector& indx,
					     const Standard_Real MinPivot = 1.e-20);

// Given a SYMMETRIC matrix a, this routine computes its 
// LU decomposition. 
// a is given through a vector of its non zero components of the upper
// triangular matrix.
// indx is the indice vector of the diagonal elements of a.
// a is replaced by its LU decomposition.
// The range of the matrix is n = indx.Length(), 
// and a.Length() = indx(n).



Standard_EXPORT Standard_Integer DACTCL_Solve(const math_Vector& a, math_Vector& b, 
					 const math_IntegerVector& indx, 
					 const Standard_Real MinPivot = 1.e-20);

// Solves a * x = b for a vector x and a matrix a coming from DACTCL_Decompose.
// indx is the same vector as in DACTCL_Decompose.
// the vector b is replaced by the vector solution x.




Standard_EXPORT Standard_Integer Jacobi(math_Matrix& a, math_Vector& d, math_Matrix& v, Standard_Integer& nrot);

// Computes all eigenvalues and eigenvectors of a real symmetric matrix
// a(1..n, 1..n). On output, elements of a above the diagonal are destroyed. 
// d(1..n) returns the eigenvalues of a. v(1..n, 1..n) is a matrix whose 
// columns contain, on output, the normalized eigenvectors of a. nrot returns
// the number of Jacobi rotations that were required.
// Eigenvalues are sorted into descending order, and eigenvectors are 
// arranges correspondingly.

#endif









