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

#ifndef _math_BracketMinimum_HeaderFile
#define _math_BracketMinimum_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class math_Function;


//! Given two distinct initial points, BracketMinimum
//! implements the computation of three points (a, b, c) which
//! bracket the minimum of the function and verify A less than
//! B, B less than C and F(A) less than F(B), F(B) less than (C).
class math_BracketMinimum 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Given two initial values this class computes a
  //! bracketing triplet of abscissae Ax, Bx, Cx
  //! (such that Bx is between Ax and Cx, F(Bx) is
  //! less than both F(Bx) and F(Cx)) the Brent minimization is done
  //! on the function F.
  Standard_EXPORT math_BracketMinimum(math_Function& F, const Standard_Real A, const Standard_Real B);
  

  //! Given two initial values this class computes a
  //! bracketing triplet of abscissae Ax, Bx, Cx
  //! (such that Bx is between Ax and Cx, F(Bx) is
  //! less than both F(Bx) and F(Cx)) the Brent minimization is done
  //! on the function F.
  //! This constructor has to be used if F(A) is known.
  Standard_EXPORT math_BracketMinimum(math_Function& F, const Standard_Real A, const Standard_Real B, const Standard_Real FA);
  

  //! Given two initial values this class computes a
  //! bracketing triplet of abscissae Ax, Bx, Cx
  //! (such that Bx is between Ax and Cx, F(Bx) is
  //! less than both F(Bx) and F(Cx)) the Brent minimization is done
  //! on the function F.
  //! This constructor has to be used if F(A) and F(B) are known.
  Standard_EXPORT math_BracketMinimum(math_Function& F, const Standard_Real A, const Standard_Real B, const Standard_Real FA, const Standard_Real FB);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns the bracketed triplet of abscissae.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
  Standard_EXPORT void Values (Standard_Real& A, Standard_Real& B, Standard_Real& C) const;
  
  //! returns the bracketed triplet function values.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
  Standard_EXPORT void FunctionValues (Standard_Real& FA, Standard_Real& FB, Standard_Real& FC) const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:

  
  //! Is used internally by the constructors.
  Standard_EXPORT void Perform (math_Function& F, const Standard_Real A, const Standard_Real B);




private:



  Standard_Boolean Done;
  Standard_Real Ax;
  Standard_Real Bx;
  Standard_Real Cx;
  Standard_Real FAx;
  Standard_Real FBx;
  Standard_Real FCx;
  Standard_Boolean myFA;
  Standard_Boolean myFB;


};


#include <math_BracketMinimum.lxx>





#endif // _math_BracketMinimum_HeaderFile
