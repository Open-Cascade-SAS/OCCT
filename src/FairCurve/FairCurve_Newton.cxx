// File:	FairCurve_Newton.cxx
// Created:	Fri Oct 11 10:18:04 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>


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
