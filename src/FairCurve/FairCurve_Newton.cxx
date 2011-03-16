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
// Attention cette ecriture est bancale car FairCurve_Newton::IsConverged() n'est pas
// pas utiliser dans le constructeur de NewtonMinimum !!
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
// C'est beaucoup mieux
}

Standard_Boolean FairCurve_Newton::IsConverged() const
// On converge si le pas est tres petits
// ou si le critere progresse peu avec un pas raisonnable, cette derniere exigence
// permetant de detecter les glissements infinis, 
// (cas ou le critere varie tres lentement).
{
  Standard_Real N = TheStep.Norm();
  return ( (N <= mySpTol/100 ) || 
	   ( Abs(TheMinimum-PreviousMinimum) <= XTol*Abs(PreviousMinimum)
           && N<=mySpTol) );  
}
