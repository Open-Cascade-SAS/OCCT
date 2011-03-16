// File:	GeomLib_PolyFunc.cxx
// Created:	Tue Sep 22 16:37:48 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomLib_PolyFunc.ixx>
#include <PLib.hxx>
#include <math_Vector.hxx>

GeomLib_PolyFunc::GeomLib_PolyFunc(const math_Vector& Coeffs) 
                                  :myCoeffs(1, Coeffs.Length()-1)
{ // On construit le polynome derive
  for (Standard_Integer ii=1; ii<=myCoeffs.Length(); ii++)
    myCoeffs(ii) = ii*Coeffs(ii+1);
}

 Standard_Boolean GeomLib_PolyFunc::Value(const Standard_Real X,
					  Standard_Real& F) 
{
  Standard_Real * coeff = &myCoeffs(1);
  Standard_Real * ff = &F;
  PLib::EvalPolynomial(X, 0, myCoeffs.Length()-1, 1,  coeff[0], ff[0]);
  return Standard_True;
}

 Standard_Boolean GeomLib_PolyFunc::Derivative(const Standard_Real X,
					       Standard_Real& D) 
{
  Standard_Real * coeff = &myCoeffs(1);
  math_Vector Aux(1, 2);
  Standard_Real * ff = &Aux(1);  
  PLib::EvalPolynomial(X, 1, myCoeffs.Length()-1, 1,  coeff[0], ff[0]);
  D = Aux(2);
  return Standard_True;
}

 Standard_Boolean GeomLib_PolyFunc::Values(const Standard_Real X,
					   Standard_Real& F,
					   Standard_Real& D) 
{
 Standard_Real * coeff = &myCoeffs(1);
 math_Vector Aux(1, 2);
 Standard_Real * ff = &Aux(1);  
 PLib::EvalPolynomial(X, 1, myCoeffs.Length()-1, 1,  coeff[0], ff[0]);
 F = Aux(1);
 D = Aux(2);
 return Standard_True;
}
