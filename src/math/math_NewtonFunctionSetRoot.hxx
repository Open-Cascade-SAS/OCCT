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

#ifndef _math_NewtonFunctionSetRoot_HeaderFile
#define _math_NewtonFunctionSetRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class Standard_DimensionError;
class math_FunctionSetWithDerivatives;
class math_Matrix;



//! This class computes the root of a set of N functions of N variables,
//! knowing an initial guess at the solution and using the
//! Newton Raphson algorithm. Knowledge of all the partial
//! derivatives (Jacobian) is required.
class math_NewtonFunctionSetRoot 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Initialize correctly all the fields of this class.
  //! The range (1, F.NbVariables()) must be especially respected for
  //! all vectors and matrix declarations.
  Standard_EXPORT math_NewtonFunctionSetRoot(math_FunctionSetWithDerivatives& theFunction, const math_Vector& theXTolerance, const Standard_Real theFTolerance, const Standard_Integer tehNbIterations = 100);
  

  //! This constructor should be used in a sub-class to initialize
  //! correctly all the fields of this class.
  //! The range (1, F.NbVariables()) must be especially respected for
  //! all vectors and matrix declarations.
  //! The method SetTolerance must be called before performing the algorithm.
  Standard_EXPORT math_NewtonFunctionSetRoot(math_FunctionSetWithDerivatives& theFunction, const Standard_Real theFTolerance, const Standard_Integer theNbIterations = 100);
  
  //! Destructor
  Standard_EXPORT virtual ~math_NewtonFunctionSetRoot();
  
  //! Initializes the tolerance values for the unknowns.
  Standard_EXPORT void SetTolerance (const math_Vector& XTol);
  

  //! The Newton method is done to improve the root of the function
  //! from the initial guess point. The solution is found when:
  //! abs(Xj - Xj-1)(i) <= XTol(i) and abs(Fi) <= FTol for all i;
  Standard_EXPORT void Perform (math_FunctionSetWithDerivatives& theFunction, const math_Vector& theStartingPoint);
  

  //! The Newton method is done to improve the root of the function
  //! from the initial guess point. Bounds may be given, to constrain the solution.
  //! The solution is found when:
  //! abs(Xj - Xj-1)(i) <= XTol(i) and abs(Fi) <= FTol for all i;
  Standard_EXPORT void Perform (math_FunctionSetWithDerivatives& theFunction, const math_Vector& theStartingPoint, const math_Vector& theInfBound, const math_Vector& theSupBound);
  

  //! This method is called at the end of each iteration to check if the
  //! solution is found.
  //! Vectors DeltaX, Fvalues and Jacobian Matrix are consistent with the
  //! possible solution Vector Sol and can be inspected to decide whether
  //! the solution is reached or not.
    virtual Standard_Boolean IsSolutionReached (math_FunctionSetWithDerivatives& F);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns the value of the root of function F.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
    const math_Vector& Root() const;
  
  //! outputs the root vector in Root.
  //! Exception NotDone is raised if the root was not found.
  //! Exception DimensionError is raised if the range of Root is
  //! not equal to the range of the StartingPoint.
    void Root (math_Vector& Root) const;
  
  //! Outputs the state number associated with the solution
  //! vector root.
    Standard_Integer StateNumber() const;
  
  //! Returns the matrix value of the derivative at the root.
  //! Exception NotDone is raised if the root was not found.
    const math_Matrix& Derivative() const;
  
  //! Outputs the matrix value of the derivative at the root in
  //! Der.
  //! Exception NotDone is raised if the root was not found.
  //! Exception DimensionError is raised if the range of Der is
  //! not equal to the range of the StartingPoint.
    void Derivative (math_Matrix& Der) const;
  
  //! Returns the vector value of the error done on the
  //! functions at the root.
  //! Exception NotDone is raised if the root was not found.
    const math_Vector& FunctionSetErrors() const;
  
  //! Outputs the vector value of the error done on the
  //! functions at the root in Err.
  //! Exception NotDone is raised if the root was not found.
  //! Exception DimensionError is raised if the range of Err is
  //! not equal to the range of the StartingPoint.
    void FunctionSetErrors (math_Vector& Err) const;
  
  //! Returns the number of iterations really done
  //! during the computation of the Root.
  //! Exception NotDone is raised if the root was not found.
    Standard_Integer NbIterations() const;
  
  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:



  math_Vector TolX;
  Standard_Real TolF;
  math_IntegerVector Indx;
  math_Vector Scratch;
  math_Vector Sol;
  math_Vector DeltaX;
  math_Vector FValues;
  math_Matrix Jacobian;


private:



  Standard_Boolean Done;
  Standard_Integer State;
  Standard_Integer Iter;
  Standard_Integer Itermax;


};


#include <math_NewtonFunctionSetRoot.lxx>





#endif // _math_NewtonFunctionSetRoot_HeaderFile
