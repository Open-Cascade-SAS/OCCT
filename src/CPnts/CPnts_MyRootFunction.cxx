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
				const Standard_Address D,
				const Standard_Integer Order)
{
  myFunction.Init(F,D);
  myOrder = Order;
}

void CPnts_MyRootFunction::Init(const Standard_Real X0,
				const Standard_Real L) 
{
  myX0 = X0;
  myL = L;
  myTol = -1; //to supress the tolerance
}

void CPnts_MyRootFunction::Init(const Standard_Real X0,
				const Standard_Real L,
				const Standard_Real Tol) 
{
  myX0 = X0;
  myL = L;
  myTol = Tol;
}

Standard_Boolean CPnts_MyRootFunction::Value(const Standard_Real X,
					     Standard_Real& F)
{
  math_GaussSingleIntegration Length;

  if (myTol <= 0) Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder);
  else Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder, myTol);

  if (Length.IsDone()){ 
    F= Length.Value() - myL; 
    return Standard_True;
  }
  else {
    return Standard_False;
  }
} 

Standard_Boolean CPnts_MyRootFunction::Derivative(const Standard_Real X,  
						  Standard_Real& Df)
{
  return myFunction.Value(X,Df);
}

Standard_Boolean CPnts_MyRootFunction::Values(const Standard_Real X, 
					      Standard_Real& F, 
					      Standard_Real& Df)
{
  math_GaussSingleIntegration Length;

  if (myTol <= 0) Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder);
  else Length = math_GaussSingleIntegration(myFunction, myX0, X, myOrder, myTol);

  if (Length.IsDone()){ 
    F= Length.Value() - myL; 
    return myFunction.Value(X,Df);
  }
  else {
    return Standard_False;
  }
}
