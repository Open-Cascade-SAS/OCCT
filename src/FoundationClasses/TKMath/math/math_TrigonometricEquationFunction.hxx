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
//! a*std::cos(x)*std::cos(x) + 2*b*std::cos(x)*Sin(x) + c*std::cos(x) + d*Sin(x) + e = 0
//! See class math_TrigonometricFunctionRoots
class math_TrigonometricEquationFunction : public math_FunctionWithDerivative
{
  double myAA;
  double myBB;
  double myCC;
  double myDD;
  double myEE;

public:
  math_TrigonometricEquationFunction(const double A,
                                     const double B,
                                     const double C,
                                     const double D,
                                     const double E)
      : myAA(A),
        myBB(B),
        myCC(C),
        myDD(D),
        myEE(E)
  {
  }

  bool Value(const double X, double& F)
  {
    double CN = cos(X), SN = sin(X);
    //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
    F = CN * (myAA * CN + (myBB + myBB) * SN + myCC) + myDD * SN + myEE;
    return true;
  }

  bool Derivative(const double X, double& D)
  {
    double CN = std::cos(X), SN = std::sin(X);
    //-- D = -2*AA*CN*SN+2*BB*(CN*CN-SN*SN)-CC*SN+DD*CN;
    D = -myAA * CN * SN + myBB * (CN * CN - SN * SN);
    D += D;
    D += -myCC * SN + myDD * CN;
    return true;
  }

  bool Values(const double X, double& F, double& D)
  {
    double CN = std::cos(X), SN = std::sin(X);
    //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
    //-- D = -2*AA*CN*SN+2*BB*(CN*CN-SN*SN)-CC*SN+DD*CN;
    double AACN = myAA * CN;
    double BBSN = myBB * SN;

    F = AACN * CN + BBSN * (CN + CN) + myCC * CN + myDD * SN + myEE;
    D = -AACN * SN + myBB * (CN * CN - SN * SN);
    D += D;
    D += -myCC * SN + myDD * CN;
    return true;
  }
};

#endif // _math_TrigonometricEquationFunction_HeaderFile
