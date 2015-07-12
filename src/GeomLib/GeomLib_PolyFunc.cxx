// Created on: 1998-09-22
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <GeomLib_PolyFunc.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>

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
