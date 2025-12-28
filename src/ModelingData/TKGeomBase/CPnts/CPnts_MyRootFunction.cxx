// Copyright (c) 1995-1999 Matra Datavision
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

#include <CPnts_MyRootFunction.hxx>
#include <math_GaussSingleIntegration.hxx>
#include <Standard_DomainError.hxx>

void CPnts_MyRootFunction::Init(const CPnts_RealFunction& F,
                                void* const    D,
                                const int    Order)
{
  myFunction.Init(F, D);
  myOrder = Order;
}

void CPnts_MyRootFunction::Init(const double X0, const double L)
{
  myX0  = X0;
  myL   = L;
  myTol = -1; // to suppress the tolerance
}

void CPnts_MyRootFunction::Init(const double X0,
                                const double L,
                                const double Tol)
{
  myX0  = X0;
  myL   = L;
  myTol = Tol;
}

bool CPnts_MyRootFunction::Value(const double X, double& F)
{
  math_GaussSingleIntegration Length;

  if (myTol <= 0)
    Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder);
  else
    Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder, myTol);

  if (Length.IsDone())
  {
    F = Length.Value() - myL;
    return true;
  }
  else
  {
    return false;
  }
}

bool CPnts_MyRootFunction::Derivative(const double X, double& Df)
{
  return myFunction.Value(X, Df);
}

bool CPnts_MyRootFunction::Values(const double X,
                                              double&      F,
                                              double&      Df)
{
  math_GaussSingleIntegration Length;

  if (myTol <= 0)
    Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder);
  else
    Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder, myTol);

  if (Length.IsDone())
  {
    F = Length.Value() - myL;
    return myFunction.Value(X, Df);
  }
  else
  {
    return false;
  }
}
