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


#include <FairCurve_Newton.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>

//=======================================================================
//function : FairCurve_Newton
//purpose  : Constructor
//=======================================================================
FairCurve_Newton::FairCurve_Newton(
  const math_MultipleVarFunctionWithHessian& theFunction,
  const Standard_Real                        theSpatialTolerance, 
  const Standard_Real                        theCriteriumTolerance, 
  const Standard_Integer                     theNbIterations, 
  const Standard_Real                        theConvexity, 
  const Standard_Boolean                     theWithSingularity
  )
: math_NewtonMinimum(theFunction,
                     theCriteriumTolerance,
                     theNbIterations,
                     theConvexity,
                     theWithSingularity),
  mySpTol(theSpatialTolerance)
{
}

//=======================================================================
//function : IsConverged
//purpose  : Convert if the steps are too small or if the criterion
//           progresses little with a reasonable step, this last
//           requirement allows detecting infinite slidings
//           (case when the criterion varies troo slowly).
//=======================================================================
Standard_Boolean FairCurve_Newton::IsConverged() const
{
  const Standard_Real N = TheStep.Norm();
  return ( N <= 0.01 * mySpTol ) || ( N <= mySpTol &&
    Abs(TheMinimum - PreviousMinimum) <= XTol * Abs(PreviousMinimum));
}
