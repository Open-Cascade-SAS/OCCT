// Copyright (c) 1995-1999 Matra Datavision
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

#include <CPnts_MyRootFunction.ixx>

#include <Standard_DomainError.hxx>
#include <math_GaussSingleIntegration.hxx>



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
