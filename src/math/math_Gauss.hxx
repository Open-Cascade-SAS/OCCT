// Created on: 1991-05-13
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

#ifndef _math_Gauss_HeaderFile
#define _math_Gauss_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Matrix.hxx>
#include <math_IntegerVector.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>
#include <Message_ProgressRange.hxx>


//! This class implements the Gauss LU decomposition (Crout algorithm)
//! with partial pivoting (rows interchange) of a square matrix and
//! the different possible derived calculation :
//! - solution of a set of linear equations.
//! - inverse of a matrix.
//! - determinant of a matrix.
class math_Gauss 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Given an input n X n matrix A this constructor performs its LU
  //! decomposition with partial pivoting (interchange of rows).
  //! This LU decomposition is stored internally and may be used to
  //! do subsequent calculation.
  //! If the largest pivot found is less than MinPivot the matrix A is
  //! considered as singular.
  //! Exception NotSquare is raised if A is not a square matrix.
  Standard_EXPORT math_Gauss(const math_Matrix& A, 
                             const Standard_Real MinPivot = 1.0e-20, 
                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Returns true if the computations are successful, otherwise returns false
  Standard_Boolean IsDone() const { return Done; }

  //! Given the input Vector B this routine returns the solution X of the set
  //! of linear equations A . X = B.
  //! Exception NotDone is raised if the decomposition of A was not done
  //! successfully.
  //! Exception DimensionError is raised if the range of B is not
  //! equal to the number of rows of A.
  Standard_EXPORT void Solve (const math_Vector& B, math_Vector& X) const;

  //! Given the input Vector B this routine solves the set of linear
  //! equations A . X = B. B is replaced by the vector solution X.
  //! Exception NotDone is raised if the decomposition of A was not done
  //! successfully.
  //! Exception DimensionError is raised if the range of B is not
  //! equal to the number of rows of A.
  Standard_EXPORT void Solve (math_Vector& B) const;

  //! This routine returns the value of the determinant of the previously LU
  //! decomposed matrix A.
  //! Exception NotDone may be raised if the decomposition of A was not done
  //! successfully, zero is returned if the matrix A was considered as singular.
  Standard_EXPORT Standard_Real Determinant() const;

  //! This routine outputs Inv the inverse of the previously LU decomposed
  //! matrix A.
  //! Exception DimensionError is raised if the ranges of B are not
  //! equal to the ranges of A.
  Standard_EXPORT void Invert (math_Matrix& Inv) const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;

protected:

  math_Matrix LU;
  math_IntegerVector Index;
  Standard_Real D;
  Standard_Boolean Done;

};

inline Standard_OStream& operator<<(Standard_OStream& o, const math_Gauss& mG)
{
  mG.Dump(o);
  return o;
}

#endif // _math_Gauss_HeaderFile
