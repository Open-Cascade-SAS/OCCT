// File:	FEmTool_ElementsOfRefMatrix.cxx
// Created:	Tue Nov 10 12:58:16 1998
// Author:	Igor FEOKTISTOV
//		<ifv@paradox.nnov.matra-dtv.fr>


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

