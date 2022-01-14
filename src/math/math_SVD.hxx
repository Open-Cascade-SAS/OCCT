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

#ifndef _math_SVD_HeaderFile
#define _math_SVD_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>


//! SVD implements the solution of a set of N linear equations
//! of M unknowns without condition on N or M. The Singular
//! Value Decomposition algorithm is used. For singular or
//! nearly singular matrices SVD is a better choice than Gauss
//! or GaussLeastSquare.
class math_SVD 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Given as input an n X m matrix A with n < m, n = m or n > m
  //! this constructor performs the Singular Value Decomposition.
  Standard_EXPORT math_SVD(const math_Matrix& A);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  

  //! Given the input Vector B this routine solves the set of linear
  //! equations A . X = B.
  //! Exception NotDone is raised if the decomposition of A was not done
  //! successfully.
  //! Exception DimensionError is raised if the range of B is not
  //! equal to the rowrange of A.
  //! Exception DimensionError is raised if the range of X is not
  //! equal to the colrange of A.
  Standard_EXPORT void Solve (const math_Vector& B, math_Vector& X, const Standard_Real Eps = 1.0e-6);
  
  //! Computes the inverse Inv of matrix A such as A * Inverse = Identity.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
  //! Standard_DimensionError if the ranges of Inv are
  //! compatible with the ranges of A.
  Standard_EXPORT void PseudoInverse (math_Matrix& Inv, const Standard_Real Eps = 1.0e-6);
  
  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Standard_Boolean Done;
  math_Matrix U;
  math_Matrix V;
  math_Vector Diag;
  Standard_Integer RowA;


};


#include <math_SVD.lxx>





#endif // _math_SVD_HeaderFile
