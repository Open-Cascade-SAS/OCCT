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

#ifndef _math_GaussLeastSquare_HeaderFile
#define _math_GaussLeastSquare_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <math_Matrix.hxx>
#include <math_IntegerVector.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class Standard_DimensionError;
class math_Matrix;



//! This class implements the least square solution of a set of
//! n linear equations of m unknowns (n >= m) using the gauss LU
//! decomposition algorithm.
//! This algorithm is more likely subject to numerical instability
//! than math_SVD.
class math_GaussLeastSquare 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Given an input n X m matrix A with n >= m this constructor
  //! performs the LU decomposition with partial pivoting
  //! (interchange of rows) of the matrix AA = A.Transposed() * A;
  //! This LU decomposition is stored internally and may be used
  //! to do subsequent calculation.
  //! If the largest pivot found is less than MinPivot the matrix <A>
  //! is considered as singular.
  Standard_EXPORT math_GaussLeastSquare(const math_Matrix& A, const Standard_Real MinPivot = 1.0e-20);
  
  //! Returns true if the computations are successful, otherwise returns false.e
    Standard_Boolean IsDone() const;
  
  //! Given the input Vector <B> this routine solves the set
  //! of linear equations A . X = B.
  //! Exception NotDone is raised if the decomposition of A was
  //! not done successfully.
  //! Exception DimensionError is raised if the range of B Inv is
  //! not equal to the rowrange of A.
  //! Exception DimensionError is raised if the range of X Inv is
  //! not equal to the colrange of A.
  Standard_EXPORT void Solve (const math_Vector& B, math_Vector& X) const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:



  Standard_Boolean Singular;
  math_Matrix LU;
  math_Matrix A2;
  math_IntegerVector Index;
  Standard_Real D;


private:



  Standard_Boolean Done;


};


#include <math_GaussLeastSquare.lxx>





#endif // _math_GaussLeastSquare_HeaderFile
