// Created on: 1996-02-28
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _math_NewtonMinimum_HeaderFile
#define _math_NewtonMinimum_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Precision.hxx>
#include <Standard_Boolean.hxx>
#include <math_Status.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class Standard_DimensionError;
class math_MultipleVarFunctionWithHessian;



class math_NewtonMinimum 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The tolerance required on the solution is given by Tolerance.
  //! Iteration are  stopped if (!WithSingularity) and H(F(Xi)) is not definite
  //! positive (if the smaller eigenvalue of H < Convexity)
  //! or IsConverged() returns True for 2 successives Iterations.
  //! Warning: This constructor does not perform computation.
  Standard_EXPORT math_NewtonMinimum(const math_MultipleVarFunctionWithHessian& theFunction,
                                     const Standard_Real theTolerance = Precision::Confusion(),
                                     const Standard_Integer theNbIterations = 40,
                                     const Standard_Real theConvexity = 1.0e-6,
                                     const Standard_Boolean theWithSingularity = Standard_True);
  
  //! Search the solution.
  Standard_EXPORT void Perform (math_MultipleVarFunctionWithHessian& theFunction, const math_Vector& theStartingPoint);
  
  //! Destructor
  Standard_EXPORT virtual ~math_NewtonMinimum();
  

  //! This method is called at the end of each iteration to check the convergence:
  //! || Xi+1 - Xi || < Tolerance or || F(Xi+1) - F(Xi)|| < Tolerance * || F(Xi) ||
  //! It can be redefined in a sub-class to implement a specific test.
    virtual Standard_Boolean IsConverged() const;
  
  //! Tests if an error has occurred.
    Standard_Boolean IsDone() const;
  
  //! Tests if the Function is convexe during optimization.
    Standard_Boolean IsConvex() const;
  
  //! returns the location vector of the minimum.
  //! Exception NotDone is raised if an error has occurred.
    const math_Vector& Location() const;
  
  //! outputs the location vector of the minimum in Loc.
  //! Exception NotDone is raised if an error has occurred.
  //! Exception DimensionError is raised if the range of Loc is not
  //! equal to the range of the StartingPoint.
    void Location (math_Vector& Loc) const;
  
  //! Set boundaries.
  Standard_EXPORT void SetBoundary (const math_Vector& theLeftBorder, const math_Vector& theRightBorder);
  
  //! returns the value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Real Minimum() const;
  
  //! returns the gradient vector at the minimum.
  //! Exception NotDone is raised if an error has occurred.
  //! The minimum was not found.
    const math_Vector& Gradient() const;
  
  //! outputs the gradient vector at the minimum in Grad.
  //! Exception NotDone is raised if the minimum was not found.
  //! Exception DimensionError is raised if the range of Grad is not
  //! equal to the range of the StartingPoint.
    void Gradient (math_Vector& Grad) const;
  
  //! returns the number of iterations really done in the
  //! calculation of the minimum.
  //! The exception NotDone is raised if an error has occurred.
    Standard_Integer NbIterations() const;

  //! Returns the Status of computation.
  //! The exception NotDone is raised if an error has occurred.
    math_Status GetStatus() const;

  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:



  math_Status TheStatus;
  math_Vector TheLocation;
  math_Vector TheGradient;
  math_Vector TheStep;
  math_Matrix TheHessian;
  Standard_Real PreviousMinimum;
  Standard_Real TheMinimum;
  Standard_Real MinEigenValue;
  Standard_Real XTol;
  Standard_Real CTol;
  Standard_Integer nbiter;
  Standard_Boolean NoConvexTreatement;
  Standard_Boolean Convex;
  Standard_Boolean myIsBoundsDefined;
  math_Vector myLeft;
  math_Vector myRight;


private:



  Standard_Boolean Done;
  Standard_Integer Itermax;


};


#include <math_NewtonMinimum.lxx>





#endif // _math_NewtonMinimum_HeaderFile
