// Created on: 1998-11-10
// Created by: Igor FEOKTISTOV
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



#include <FEmTool_ElementsOfRefMatrix.ixx>
#include <PLib_Base.hxx>
#include <TColStd_Array1OfReal.hxx>


FEmTool_ElementsOfRefMatrix::FEmTool_ElementsOfRefMatrix(const Handle(PLib_Base)& TheBase,
							 const Standard_Integer DerOrder):
       myBase(TheBase)
{
  if(DerOrder < 0 || DerOrder > 3) 
    Standard_ConstructionError::Raise("FEmTool_ElementsOfRefMatrix");

  myDerOrder = DerOrder;
  myNbEquations = (myBase->WorkDegree()+2)*(myBase->WorkDegree()+1)/2;

}

Standard_Integer FEmTool_ElementsOfRefMatrix::NbVariables() const
{
  return 1;
}

Standard_Integer FEmTool_ElementsOfRefMatrix::NbEquations() const
{
  return myNbEquations;
}

Standard_Boolean FEmTool_ElementsOfRefMatrix::Value(const math_Vector& X, math_Vector& F) 
{
  if(F.Length() < myNbEquations) Standard_OutOfRange::Raise("FEmTool_ElementsOfRefMatrix::Value");

  Standard_Real u = X(X.Lower());
  TColStd_Array1OfReal Basis(0,myBase->WorkDegree()), Aux(0,myBase->WorkDegree());

  switch (myDerOrder) {
  case 0 :
    myBase->D0(u, Basis);
    break;
  case 1 :
    myBase->D1(u, Aux, Basis);
    break;
  case 2 :
    myBase->D2(u, Aux, Aux, Basis);
    break;
  case 3 :
    myBase->D3(u, Aux, Aux, Aux, Basis);
    break; 
  }
 
  Standard_Integer i, j, ii = 0;
  for(i = 0; i<=myBase->WorkDegree(); i++)
    for(j = i; j<=myBase->WorkDegree(); j++) {
      F(F.Lower()+ii) = Basis(i)*Basis(j); ii++;
    }

  return Standard_True;
}

