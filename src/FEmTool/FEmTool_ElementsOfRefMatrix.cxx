// Created on: 1998-11-10
// Created by: Igor FEOKTISTOV
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


#include <FEmTool_ElementsOfRefMatrix.hxx>
#include <PLib_Base.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColStd_Array1OfReal.hxx>

FEmTool_ElementsOfRefMatrix::FEmTool_ElementsOfRefMatrix(const Handle(PLib_Base)& TheBase,
							 const Standard_Integer DerOrder):
       myBase(TheBase)
{
  if(DerOrder < 0 || DerOrder > 3) 
    throw Standard_ConstructionError("FEmTool_ElementsOfRefMatrix");

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
  if(F.Length() < myNbEquations) throw Standard_OutOfRange("FEmTool_ElementsOfRefMatrix::Value");

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

