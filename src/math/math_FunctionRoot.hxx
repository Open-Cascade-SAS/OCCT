// Created on: 1991-03-14
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

#ifndef _math_FunctionRoot_HeaderFile
#define _math_FunctionRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class math_FunctionWithDerivative;



//! This class implements the computation of a root of a function of
//! a single variable which is near an initial guess using a minimization
//! algorithm.Knowledge of the derivative is required. The
//! algorithm used is the same as in
class math_FunctionRoot 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The Newton-Raphson method is done to find the root of the function F
  //! from the initial guess Guess.The tolerance required on
  //! the root is given by Tolerance. Iterations are stopped if
  //! the expected solution does not stay in the range A..B.
  //! The solution is found when abs(Xi - Xi-1) <= Tolerance;
  //! The maximum number of iterations allowed is given by NbIterations.
  Standard_EXPORT math_FunctionRoot(math_FunctionWithDerivative& F, const Standard_Real Guess, const Standard_Real Tolerance, const Standard_Integer NbIterations = 100);
  

  //! The Newton-Raphson method is done to find the root of the function F
  //! from the initial guess Guess.
  //! The tolerance required on the root is given by Tolerance.
  //! Iterations are stopped if the expected solution does not stay in the
  //! range A..B
  //! The solution is found when abs(Xi - Xi-1) <= Tolerance;
  //! The maximum number of iterations allowed is given by NbIterations.
  Standard_EXPORT math_FunctionRoot(math_FunctionWithDerivative& F, const Standard_Real Guess, const Standard_Real Tolerance, const Standard_Real A, const Standard_Real B, const Standard_Integer NbIterations = 100);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! returns the value of the root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Real Root() const;
  
  //! returns the value of the derivative at the root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Real Derivative() const;
  
  //! returns the value of the function at the root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Real Value() const;
  
  //! returns the number of iterations really done on the
  //! computation of the Root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Integer NbIterations() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Standard_Boolean Done;
  Standard_Real TheRoot;
  Standard_Real TheError;
  Standard_Real TheDerivative;
  Standard_Integer NbIter;


};


#include <math_FunctionRoot.lxx>





#endif // _math_FunctionRoot_HeaderFile
