// Created on: 1991-05-13
// Created by: Laurent Painnot
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _math_TrigonometricEquationFunction_HeaderFile
#define _math_TrigonometricEquationFunction_HeaderFile

#include <math_FunctionWithDerivative.hxx>

//! This is function, which corresponds trigonometric equation
//! a*Cos(x)*Cos(x) + 2*b*Cos(x)*Sin(x) + c*Cos(x) + d*Sin(x) + e = 0
//! See class math_TrigonometricFunctionRoots
class math_TrigonometricEquationFunction : public math_FunctionWithDerivative
{
  Standard_Real myAA;
  Standard_Real myBB;
  Standard_Real myCC;
  Standard_Real myDD;
  Standard_Real myEE;

public:

  math_TrigonometricEquationFunction(const Standard_Real A,
    const Standard_Real B,
    const Standard_Real C,
    const Standard_Real D,
    const Standard_Real E)
    : myAA(A), myBB(B), myCC(C), myDD(D), myEE(E)
  {
  }

  Standard_Boolean Value(const Standard_Real X, Standard_Real& F) {
    Standard_Real CN = cos(X), SN = sin(X);
    //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
    F = CN*(myAA*CN + (myBB + myBB)*SN + myCC) + myDD*SN + myEE;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real X, Standard_Real& D) {
    Standard_Real CN = Cos(X), SN = Sin(X);
    //-- D = -2*AA*CN*SN+2*BB*(CN*CN-SN*SN)-CC*SN+DD*CN;
    D = -myAA*CN*SN + myBB*(CN*CN - SN*SN);
    D += D;
    D += -myCC*SN + myDD*CN;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real X, Standard_Real& F, Standard_Real& D) {
    Standard_Real CN = Cos(X), SN = Sin(X);
    //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
    //-- D = -2*AA*CN*SN+2*BB*(CN*CN-SN*SN)-CC*SN+DD*CN;
    Standard_Real AACN = myAA*CN;
    Standard_Real BBSN = myBB*SN;

    F = AACN*CN + BBSN*(CN + CN) + myCC*CN + myDD*SN + myEE;
    D = -AACN*SN + myBB*(CN*CN - SN*SN);
    D += D;
    D += -myCC*SN + myDD*CN;
    return Standard_True;
  }
};

#endif // _math_TrigonometricEquationFunction_HeaderFile
