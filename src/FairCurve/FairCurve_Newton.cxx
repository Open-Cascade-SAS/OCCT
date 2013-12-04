// Created on: 1996-10-11
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <FairCurve_Newton.ixx>

FairCurve_Newton::FairCurve_Newton(math_MultipleVarFunctionWithHessian& F, 
				   const math_Vector& StartingPoint, 
				   const Standard_Real SpatialTolerance, 
				   const Standard_Real CriteriumTolerance, 
				   const Standard_Integer NbIterations, 
				   const Standard_Real Convexity, 
				   const Standard_Boolean WithSingularity)
                            :math_NewtonMinimum(F, StartingPoint, CriteriumTolerance,
						NbIterations, Convexity, WithSingularity),
			     mySpTol(SpatialTolerance)
						  
{
// Attention this writing is wrong as FairCurve_Newton::IsConverged() is not
// used in the constructor of NewtonMinimum !!
}

FairCurve_Newton::FairCurve_Newton(math_MultipleVarFunctionWithHessian& F,
				   const Standard_Real SpatialTolerance, 
				   const Standard_Real CriteriumTolerance, 
				   const Standard_Integer NbIterations, 
				   const Standard_Real Convexity, 
				   const Standard_Boolean WithSingularity)
                            :math_NewtonMinimum(F, CriteriumTolerance,
						NbIterations, Convexity, WithSingularity),
			     mySpTol(SpatialTolerance)
						  
{
// It is much better
}

Standard_Boolean FairCurve_Newton::IsConverged() const
// Convert if the steps are too small 
// or if the criterion progresses little with a reasonable step, this last requirement
// allows detecting infinite slidings, 
// (case when the criterion varies troo slowly).
{
  Standard_Real N = TheStep.Norm();
  return ( (N <= mySpTol/100 ) || 
	   ( Abs(TheMinimum-PreviousMinimum) <= XTol*Abs(PreviousMinimum)
           && N<=mySpTol) );  
}
