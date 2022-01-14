// Created on: 1991-05-14
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

#ifndef _math_Jacobi_HeaderFile
#define _math_Jacobi_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>



//! This class implements the Jacobi method to find the eigenvalues and
//! the eigenvectors of a real symmetric square matrix.
//! A sort of eigenvalues is done.
class math_Jacobi 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Given a Real n X n matrix A, this constructor computes all its
  //! eigenvalues and eigenvectors using the Jacobi method.
  //! The exception NotSquare is raised if the matrix is not square.
  //! No verification that the matrix A is really symmetric is done.
  Standard_EXPORT math_Jacobi(const math_Matrix& A);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns the eigenvalues vector.
  //! Exception NotDone is raised if calculation is not done successfully.
    const math_Vector& Values() const;
  
  //! returns the eigenvalue number Num.
  //! Eigenvalues are in the range (1..n).
  //! Exception NotDone is raised if calculation is not done successfully.
    Standard_Real Value (const Standard_Integer Num) const;
  
  //! returns the eigenvectors matrix.
  //! Exception NotDone is raised if calculation is not done successfully.
    const math_Matrix& Vectors() const;
  
  //! Returns the eigenvector V of number Num.
  //! Eigenvectors are in the range (1..n).
  //! Exception NotDone is raised if calculation is not done successfully.
    void Vector (const Standard_Integer Num, math_Vector& V) const;
  
  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Standard_Boolean Done;
  math_Matrix AA;
  Standard_Integer NbRotations;
  math_Vector EigenValues;
  math_Matrix EigenVectors;


};


#include <math_Jacobi.lxx>





#endif // _math_Jacobi_HeaderFile
