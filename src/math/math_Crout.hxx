// Created on: 1991-08-22
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

#ifndef _math_Crout_HeaderFile
#define _math_Crout_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Matrix.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class math_NotSquare;
class Standard_DimensionError;
class math_Matrix;


//! This class implements the Crout algorithm used to solve a
//! system A*X = B where A is a symmetric matrix. It can be used to
//! invert a symmetric matrix.
//! This algorithm is similar to Gauss but is faster than Gauss.
//! Only the inferior triangle of A and the diagonal can be given.
class math_Crout 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Given an input matrix A, this algorithm inverts A by the
  //! Crout algorithm. The user can give only the inferior
  //! triangle for the implementation.
  //! A can be decomposed like this:
  //! A = L * D * T(L) where L is triangular inferior and D is
  //! diagonal.
  //! If one element of A is less than MinPivot, A is
  //! considered as singular.
  //! Exception NotSquare is raised if A is not a square matrix.
  Standard_EXPORT math_Crout(const math_Matrix& A, const Standard_Real MinPivot = 1.0e-20);
  
  //! Returns True if all has been correctly done.
    Standard_Boolean IsDone() const;
  
  //! Given an input vector <B>, this routine returns the
  //! solution of the set of linear equations A . X = B.
  //! Exception NotDone is raised if the decomposition was not
  //! done successfully.
  //! Exception DimensionError is raised if the range of B is
  //! not equal to the rowrange of A.
  Standard_EXPORT void Solve (const math_Vector& B, math_Vector& X) const;
  
  //! returns the inverse matrix of A. Only the inferior
  //! triangle is returned.
  //! Exception NotDone is raised if NotDone.
    const math_Matrix& Inverse() const;
  
  //! returns in Inv the inverse matrix of A. Only the inferior
  //! triangle is returned.
  //! Exception NotDone is raised if NotDone.
    void Invert (math_Matrix& Inv) const;
  
  //! Returns the value of the determinant of the previously LU
  //! decomposed matrix A. Zero is returned if the matrix A is considered as singular.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
    Standard_Real Determinant() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  math_Matrix InvA;
  Standard_Boolean Done;
  Standard_Real Det;


};


#include <math_Crout.lxx>





#endif // _math_Crout_HeaderFile
