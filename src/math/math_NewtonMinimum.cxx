// File:	math_NewtonMinimum.cxx
// Created:	Fri May  3 09:33:31 1996
// Author:	Philippe MANGIN
//		<pmn@sgi38>

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_NewtonMinimum.ixx>

#include <math_Gauss.hxx>
#include <math_Jacobi.hxx>

//============================================================================
math_NewtonMinimum::math_NewtonMinimum(math_MultipleVarFunctionWithHessian& F, 
				       const math_Vector& StartingPoint,
				       const Standard_Real Tolerance,
				       const Standard_Integer NbIterations,
				       const Standard_Real Convexity,
				       const Standard_Boolean WithSingularity)
//============================================================================
                  : TheLocation(1, F.NbVariables()),
                    TheGradient(1, F.NbVariables()),
		    TheStep(1, F.NbVariables(), 10*Tolerance),
                    TheHessian(1, F.NbVariables(), 1, F.NbVariables() )
{
       XTol = Tolerance;
       CTol = Convexity;
       Itermax = NbIterations;
       NoConvexTreatement = WithSingularity;
       Convex = Standard_True;
//       Done = Standard_True;
//       TheStatus = math_OK;
       Perform ( F, StartingPoint);
}

//============================================================================
math_NewtonMinimum::math_NewtonMinimum(math_MultipleVarFunctionWithHessian& F,
				       const Standard_Real Tolerance,
				       const Standard_Integer NbIterations,
				       const Standard_Real Convexity,
				       const Standard_Boolean WithSingularity)
//============================================================================
                  : TheLocation(1, F.NbVariables()),
                    TheGradient(1, F.NbVariables()),
		    TheStep(1, F.NbVariables(), 10*Tolerance),
                    TheHessian(1, F.NbVariables(), 1, F.NbVariables() )
{
       XTol = Tolerance;
       CTol = Convexity;
       Itermax = NbIterations;
       NoConvexTreatement = WithSingularity;
       Convex = Standard_True;
       Done = Standard_False;
       TheStatus = math_NotBracketed;
}
//============================================================================
void math_NewtonMinimum::Delete()
{}
//============================================================================
void math_NewtonMinimum::Perform(math_MultipleVarFunctionWithHessian& F,
				 const math_Vector& StartingPoint)
//============================================================================
{
  math_Vector Point1 (1, F.NbVariables());
  Point1 =  StartingPoint;
  math_Vector Point2(1, F.NbVariables());
  math_Vector* precedent = &Point1;
  math_Vector* suivant = &Point2;
  math_Vector* auxiliaire = precedent;

  Standard_Boolean Ok = Standard_True;
  Standard_Integer NbConv = 0, ii, Nreduction;
  Standard_Real    VPrecedent, VItere; 

  Done = Standard_True;
  TheStatus = math_OK;
  nbiter = 0;

  while ( Ok && (NbConv < 2) ) {
    nbiter++;

    // Positionnement

    Ok = F.Values(*precedent, VPrecedent, TheGradient, TheHessian);
    if (!Ok) {
       Done = Standard_False;
       TheStatus = math_FunctionError;
       return;
    }
    if (nbiter==1) {
      PreviousMinimum =  VPrecedent;
      TheMinimum =  VPrecedent;
    }
    
    // Traitement de la non convexite

    math_Jacobi CalculVP(TheHessian);
    if ( !CalculVP.IsDone() ) {
       Done = Standard_False;
       TheStatus = math_FunctionError;
       return;
    }

        
    
    MinEigenValue = CalculVP.Values() ( CalculVP.Values().Min());
    if ( MinEigenValue < CTol) {
       Convex = Standard_False;
       if (NoConvexTreatement) {
           Standard_Real Delta = CTol+0.1*Abs(MinEigenValue) -MinEigenValue ;
           for (ii=1; ii<=TheGradient.Length(); ii++) {
               TheHessian(ii, ii) += Delta;
	     }
	 }
       else {
           TheStatus = math_FunctionError;
           return;
       }
     }

    // Schemas de Newton

    math_Gauss LU(TheHessian, CTol/100);
    if ( !LU.IsDone()) {
      Done = Standard_False;
      TheStatus = math_DirectionSearchError;
      return;
    }
   
    LU.Solve(TheGradient, TheStep);
    *suivant = *precedent - TheStep;


    //  Gestion de la convergence

    F.Value(*suivant, TheMinimum);

    if (IsConverged()) { NbConv++; }
    else               { NbConv=0; }

    //  Controle et corrections.

    VItere = TheMinimum;
    TheMinimum = PreviousMinimum;
    Nreduction =0;
    while (VItere > VPrecedent && Nreduction < 10) {
        TheStep *= 0.4;   
	*suivant = *precedent - TheStep;
	F.Value(*suivant, VItere);
	Nreduction++;
    }

    if (VItere <= VPrecedent) {
       auxiliaire =  precedent;
       precedent = suivant;
       suivant = auxiliaire;
       PreviousMinimum = VPrecedent;
       TheMinimum = VItere;
       Ok = (nbiter < Itermax);
       if (!Ok && NbConv < 2) TheStatus = math_TooManyIterations;
    } 
    else {       
       Ok = Standard_False;
       TheStatus = math_DirectionSearchError;
    }
  }
 TheLocation = *precedent;    
}

//============================================================================
Standard_Boolean math_NewtonMinimum::IsConverged() const 
//============================================================================
{
  return ( (TheStep.Norm() <= XTol ) || 
	 ( Abs(TheMinimum-PreviousMinimum) <= XTol*Abs(PreviousMinimum) ));
}

//============================================================================
void math_NewtonMinimum::Dump(Standard_OStream& o) const 
//============================================================================
{
       o<< "math_Newton Optimisation: ";
         o << " Done   ="  << Done << endl; 
         o << " Status = " << (Standard_Integer)TheStatus << endl;
	 o <<" Location Vector = " << Location() << endl;
	 o <<" Minimum value = "<< Minimum()<< endl;
	 o <<" Previous value = "<< PreviousMinimum << endl;
	 o <<" Number of iterations = " <<NbIterations() << endl;
	 o <<" Convexity = " << Convex << endl;
         o <<" Eigen Value = " << MinEigenValue << endl;
}

