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

#ifndef _math_BFGS_HeaderFile
#define _math_BFGS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Status.hxx>
#include <math_Vector.hxx>
#include <Standard_OStream.hxx>
class math_MultipleVarFunctionWithGradient;



//! This class implements the Broyden-Fletcher-Goldfarb-Shanno variant of
//! Davidson-Fletcher-Powell minimization algorithm of a function of
//! multiple variables.Knowledge of the function's gradient is required.
//!
//! It is possible to solve conditional optimization problem on hyperparallelepiped.
//! Method SetBoundary is used to define hyperparallelepiped borders. With boundaries
//! defined, the algorithm will not make evaluations of the function outside of the
//! borders.
class math_BFGS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes the computation of the minimum of a function with
  //! NbVariables.
  //! Tolerance, ZEPS and NbIterations are described in the method Perform.
  //! Warning:
  //! A call to the Perform method must be made after this
  //! initialization to effectively compute the minimum of the
  //! function F.
  Standard_EXPORT math_BFGS(const Standard_Integer NbVariables, const Standard_Real Tolerance = 1.0e-8, const Standard_Integer NbIterations = 200, const Standard_Real ZEPS = 1.0e-12);

  Standard_EXPORT virtual ~math_BFGS();

  //! Set boundaries for conditional optimization.
  //! The expected indices range of vectors is [1, NbVariables].
  Standard_EXPORT void SetBoundary(const math_Vector& theLeftBorder, const math_Vector& theRightBorder);

  //! Given the starting point StartingPoint,
  //! minimization is done on the function F.
  //! The solution F = Fi is found when :
  //! 2.0 * abs(Fi - Fi-1) <= Tolerance * (abs(Fi) + abs(Fi-1) + ZEPS).
  //! Tolerance, ZEPS and maximum number of iterations are given
  //! in the constructor.
  Standard_EXPORT void Perform (math_MultipleVarFunctionWithGradient& F, const math_Vector& StartingPoint);
  

  //! This method is called at the end of each iteration to check if the
  //! solution is found.
  //! It can be redefined in a sub-class to implement a specific test to
  //! stop the iterations.
  Standard_EXPORT virtual Standard_Boolean IsSolutionReached (math_MultipleVarFunctionWithGradient& F) const;
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! returns the location vector of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    const math_Vector& Location() const;
  
  //! outputs the location vector of the minimum in Loc.
  //! Exception NotDone is raised if the minimum was not found.
  //! Exception DimensionError is raised if the range of Loc is not
  //! equal to the range of the StartingPoint.
    void Location (math_Vector& Loc) const;
  
  //! returns the value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Real Minimum() const;
  
  //! Returns the gradient vector at the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    const math_Vector& Gradient() const;
  
  //! Returns the value of the gradient vector at the minimum in Grad.
  //! Exception NotDone is raised if the minimum was not found.
  //! Exception DimensionError is raised if the range of Grad is not
  //! equal to the range of the StartingPoint.
    void Gradient (math_Vector& Grad) const;
  
  //! Returns the number of iterations really done in the
  //! calculation of the minimum.
  //! The exception NotDone is raised if the minimum was not found.
    Standard_Integer NbIterations() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:



  math_Status TheStatus;
  math_Vector TheLocation;
  math_Vector TheGradient;
  Standard_Real PreviousMinimum;
  Standard_Real TheMinimum;
  Standard_Real XTol;
  Standard_Real EPSZ;
  Standard_Integer nbiter;
  Standard_Boolean myIsBoundsDefined;
  math_Vector myLeft;
  math_Vector myRight;


private:



  Standard_Boolean Done;
  Standard_Integer Itermax;


};


#include <math_BFGS.lxx>





#endif // _math_BFGS_HeaderFile
