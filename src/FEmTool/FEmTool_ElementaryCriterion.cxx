// File:	FEmTool_ElementaryCriterion.cxx
// Created:	Thu Nov  5 11:28:29 1998
// Author:	Igor FEOKTISTOV
//		<ifv@paradox.nnov.matra-dtv.fr>


#include <FEmTool_ElementaryCriterion.ixx>

void FEmTool_ElementaryCriterion::Set(const Handle(TColStd_HArray2OfReal)& Coeff) 
{
  myCoeff = Coeff;
}

void FEmTool_ElementaryCriterion::Set(const Standard_Real FirstKnot, const Standard_Real LastKnot) 
{
  myFirst = FirstKnot; myLast = LastKnot;
}


