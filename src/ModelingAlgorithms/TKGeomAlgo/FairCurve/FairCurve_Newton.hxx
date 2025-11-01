// Created on: 1996-10-11
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

#ifndef _FairCurve_Newton_HeaderFile
#define _FairCurve_Newton_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_NewtonMinimum.hxx>
class math_MultipleVarFunctionWithHessian;

//! Algorithme of Optimization used to make "FairCurve"
class FairCurve_Newton : public math_NewtonMinimum
{
public:
  DEFINE_STANDARD_ALLOC

  //! The tolerance  required on  the  solution is given  by Tolerance.
  //! Iteration are  stopped if (!WithSingularity)  and H(F(Xi)) is not definite
  //! positive  (if the smaller eigenvalue of H < Convexity)
  //! or IsConverged() returns True for 2 successives Iterations.
  //! Warning: This constructor do not computation
  Standard_EXPORT FairCurve_Newton(const math_MultipleVarFunctionWithHessian& theFunction,
                                   const Standard_Real    theSpatialTolerance   = 1.0e-7,
                                   const Standard_Real    theCriteriumTolerance = 1.0e-7,
                                   const Standard_Integer theNbIterations       = 40,
                                   const Standard_Real    theConvexity          = 1.0e-6,
                                   const Standard_Boolean theWithSingularity    = Standard_True);

  //! This method is  called    at the end  of   each
  //! iteration to  check the convergence :
  //! || Xi+1 - Xi || < SpatialTolerance/100 Or
  //! || Xi+1 - Xi || < SpatialTolerance and
  //! |F(Xi+1) - F(Xi)| < CriteriumTolerance * |F(xi)|
  //! It can be redefined in a sub-class to implement a specific test.
  Standard_EXPORT virtual Standard_Boolean IsConverged() const Standard_OVERRIDE;

protected:
private:
  Standard_Real mySpTol;
};

#endif // _FairCurve_Newton_HeaderFile
