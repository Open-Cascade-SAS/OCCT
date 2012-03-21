// Created on: 1998-09-22
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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
