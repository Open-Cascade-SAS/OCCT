// Created on: 1991-08-07
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

#ifndef _math_Householder_HeaderFile
#define _math_Householder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>

//! This class implements the least square solution of a set of
//! linear equations of m unknowns (n >= m) using the Householder
//! method. It solves A.X = B.
//! This algorithm has more numerical stability than
//! GaussLeastSquare but is longer.
//! It must be used if the matrix is singular or nearly singular.
//! It is about 16% longer than GaussLeastSquare if there is only
//! one member B to solve.
//! It is about 30% longer if there are twenty B members to solve.
class math_Householder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Given an input matrix A with n>= m, given an input matrix B
  //! this constructor performs the least square resolution of
  //! the set of linear equations A.X = B for each column of B.
  //! If a column norm is less than EPS, the resolution can't
  //! be done.
  //! Exception DimensionError is raised if the row number of B
  //! is different from the A row number.
  Standard_EXPORT math_Householder(const math_Matrix&  A,
                                   const math_Matrix&  B,
                                   const Standard_Real EPS = 1.0e-20);

  //! Given an input matrix A with n>= m, given an input matrix B
  //! this constructor performs the least square resolution of
  //! the set of linear equations A.X = B for each column of B.
  //! If a column norm is less than EPS, the resolution can't
  //! be done.
  //! Exception DimensionError is raised if the row number of B
  //! is different from the A row number.
  Standard_EXPORT math_Householder(const math_Matrix&     A,
                                   const math_Matrix&     B,
                                   const Standard_Integer lowerArow,
                                   const Standard_Integer upperArow,
                                   const Standard_Integer lowerAcol,
                                   const Standard_Integer upperAcol,
                                   const Standard_Real    EPS = 1.0e-20);

  //! Given an input matrix A with n>= m, given an input vector B
  //! this constructor performs the least square resolution of
  //! the set of linear equations A.X = B.
  //! If a column norm is less than EPS, the resolution can't
  //! be done.
  //! Exception DimensionError is raised if the length of B
  //! is different from the A row number.
  Standard_EXPORT math_Householder(const math_Matrix&  A,
                                   const math_Vector&  B,
                                   const Standard_Real EPS = 1.0e-20);

  //! Returns true if the computations are successful, otherwise returns false.
  Standard_Boolean IsDone() const;

  //! Given the integer Index, this routine returns the
  //! corresponding least square solution sol.
  //! Exception NotDone is raised if the resolution has not be
  //! done.
  //! Exception OutOfRange is raised if Index <=0 or
  //! Index is more than the number of columns of B.
  void Value(math_Vector& sol, const Standard_Integer Index = 1) const;

  //! Returns the matrix sol of all the solutions of the system
  //! A.X = B.
  //! Exception NotDone is raised is the resolution has not be
  //! done.
  const math_Matrix& AllValues() const;

  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
  //! This method is used internally for each constructor
  //! above and can't be used directly.
  Standard_EXPORT void Perform(const math_Matrix& A, const math_Matrix& B, const Standard_Real EPS);

private:
  math_Matrix      Sol;
  math_Matrix      Q;
  Standard_Boolean Done;
  Standard_Integer mylowerArow;
  Standard_Integer myupperArow;
  Standard_Integer mylowerAcol;
  Standard_Integer myupperAcol;
};

#include <math_Householder.lxx>

#endif // _math_Householder_HeaderFile
