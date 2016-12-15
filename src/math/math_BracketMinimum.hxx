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
//! B, B less than C and F(B) less than F(A), F(B) less than F(C).
//!
//! The algorithm supports conditional optimization. By default no limits are
//! applied to the parameter change. The method SetLimits defines the allowed range.
//! If no minimum is found in limits then IsDone() will return false. The user
//! is in charge of providing A and B to be in limits.
class math_BracketMinimum
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor preparing A and B parameters only. It does not perform the job.
  Standard_EXPORT math_BracketMinimum(const Standard_Real A, const Standard_Real B);

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

  //! Set limits of the parameter. By default no limits are applied to the parameter change.
  //! If no minimum is found in limits then IsDone() will return false. The user
  //! is in charge of providing A and B to be in limits.
  void SetLimits(const Standard_Real theLeft, const Standard_Real theRight);

  //! Set function value at A
  void SetFA(const Standard_Real theValue);

  //! Set function value at B
  void SetFB(const Standard_Real theValue);

  //! The method performing the job. It is called automatically by constructors with the function.
  Standard_EXPORT void Perform(math_Function& F);

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

private:

  //! Limit the given value to become within the range [myLeft, myRight].
  Standard_Real Limited(const Standard_Real theValue) const;

  //! Limit the value of C (see Limited) and compute the function in it.
  //! If C occurs to be between A and B then swap parameters and function
  //! values of B and C.
  //! Return false in the case of C becomes equal to B or function calculation
  //! failure.
  Standard_Boolean LimitAndMayBeSwap(math_Function& F, const Standard_Real theA,
                                     Standard_Real& theB, Standard_Real& theFB,
                                     Standard_Real& theC, Standard_Real& theFC) const;

private:

  Standard_Boolean Done;
  Standard_Real Ax;
  Standard_Real Bx;
  Standard_Real Cx;
  Standard_Real FAx;
  Standard_Real FBx;
  Standard_Real FCx;
  Standard_Real myLeft;
  Standard_Real myRight;
  Standard_Boolean myIsLimited;
  Standard_Boolean myFA;
  Standard_Boolean myFB;


};


#include <math_BracketMinimum.lxx>





#endif // _math_BracketMinimum_HeaderFile
