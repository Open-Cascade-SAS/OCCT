// Created on: 1996-10-11
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
