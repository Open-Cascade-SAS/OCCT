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

#ifndef _math_NewtonFunctionRoot_HeaderFile
#define _math_NewtonFunctionRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_OStream.hxx>
class math_FunctionWithDerivative;



//! This class implements the calculation of a root of a function of
//! a single variable starting from an initial near guess using the
//! Newton algorithm. Knowledge of the derivative is required.
class math_NewtonFunctionRoot 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The Newton method is done to find the root of the function F
  //! from the initial guess Guess.
  //! The tolerance required on the root is given by Tolerance.
  //! The solution is found when :
  //! abs(Xi - Xi-1) <= EpsX and abs(F(Xi))<= EpsF
  //! The maximum number of iterations allowed is given by NbIterations.
  Standard_EXPORT math_NewtonFunctionRoot(math_FunctionWithDerivative& F, const Standard_Real Guess, const Standard_Real EpsX, const Standard_Real EpsF, const Standard_Integer NbIterations = 100);
  

  //! The Newton method is done to find the root of the function F
  //! from the initial guess Guess.
  //! The solution must be inside the interval [A, B].
  //! The tolerance required on the root is given by Tolerance.
  //! The solution is found when :
  //! abs(Xi - Xi-1) <= EpsX and abs(F(Xi))<= EpsF
  //! The maximum number of iterations allowed is given by NbIterations.
  Standard_EXPORT math_NewtonFunctionRoot(math_FunctionWithDerivative& F, const Standard_Real Guess, const Standard_Real EpsX, const Standard_Real EpsF, const Standard_Real A, const Standard_Real B, const Standard_Integer NbIterations = 100);
  
  //! is used in a sub-class to initialize correctly all the fields
  //! of this class.
  Standard_EXPORT math_NewtonFunctionRoot(const Standard_Real A, const Standard_Real B, const Standard_Real EpsX, const Standard_Real EpsF, const Standard_Integer NbIterations = 100);
  
  //! is used internally by the constructors.
  Standard_EXPORT void Perform (math_FunctionWithDerivative& F, const Standard_Real Guess);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns the value of the root of function <F>.
  //! Exception NotDone is raised if the root was not found.
    Standard_Real Root() const;
  
  //! returns the value of the derivative at the root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Real Derivative() const;
  
  //! returns the value of the function at the root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Real Value() const;
  
  //! Returns the number of iterations really done on the
  //! computation of the Root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Integer NbIterations() const;
  
  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Standard_Boolean Done;
  Standard_Real X;
  Standard_Real Fx;
  Standard_Real DFx;
  Standard_Integer It;
  Standard_Real EpsilonX;
  Standard_Real EpsilonF;
  Standard_Integer Itermax;
  Standard_Real Binf;
  Standard_Real Bsup;


};


#include <math_NewtonFunctionRoot.lxx>





#endif // _math_NewtonFunctionRoot_HeaderFile
